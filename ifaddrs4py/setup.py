#!/usr/bin/env python3
import dataclasses
import functools
import pathlib
from setuptools import (
    Extension,
    setup,
)
import subprocess
import sys
from typing import (
    List,
)

UTF_8 = "utf-8"

BASE_PATH = pathlib.Path(__file__).parent.resolve()
EXTERN_CPP_SOURCE_SDIST = BASE_PATH / "extern" / "ifaddrs4cpp"
EXTERN_CPP_SOURCE_GIT = BASE_PATH.parent / "ifaddrs4cpp"
CMAKE_BUILD_DIRECTORY = "cmake-external-build"

IS_MACOS = sys.platform == "darwin" or sys.platform == "macos"
IS_WINDOWS = sys.platform == "win32" or sys.platform == "win64" or sys.platform == "cygwin"

CMAKE = "cmake"
CTEST = "ctest"
STATIC_LIBRARY_EXTENSION = "a"
DYNAMIC_LIBRARY_EXTENSION = "so"
if IS_MACOS:
    DYNAMIC_LIBRARY_EXTENSION = "dylib"
elif IS_WINDOWS:
    CMAKE = "cmake.exe"
    CTEST = "ctest.exe"
    STATIC_LIBRARY_EXTENSION = "lib"
    DYNAMIC_LIBRARY_EXTENSION = "dll"

WHICH_COMMAND = ["command", "-v"]
if IS_WINDOWS:
    WHICH_COMMAND = ["where.exe"]
else:
    try:
        subprocess.check_output(WHICH_COMMAND + ["command"], encoding=UTF_8).strip()
    except (subprocess.CalledProcessError, FileNotFoundError) as ignore:
        WHICH_COMMAND = ["which"]


def print_fast(string):
    sys.stdout.write(f"{string}\n")
    sys.stdout.flush()


@functools.lru_cache(maxsize=128)
def which(executable: str) -> str:
    """For determining full paths to necessary system commands and installed executables"""
    try:
        value = subprocess.check_output(WHICH_COMMAND + [executable], encoding=UTF_8).strip()
        print_fast(f"Using {executable} at {value}...")
        return value
    except (subprocess.CalledProcessError, FileNotFoundError) as e:
        raise RuntimeError(
            f"Required executable `{executable}` not found on this system. Please install it before building "
            f"ifaddrs4cpp and ifaddrs4py. Error detail: {e}"
        )


@dataclasses.dataclass
class Options:
    dynamic: bool = False
    test_cpp: bool = False


def preprocess_options(argv: List[str]) -> Options:
    """For extracting custom command-line arguments from the setup command"""
    args = {
        "--dynamic": False,
        "--test-cpp": False,
    }

    to_del: List[int] = []
    for i in range(1, len(argv)):
        if argv[i] in args:
            print_fast(f"With {argv[i]}")
            args[argv[i]] = True
            to_del.append(i)
    for i in reversed(to_del):
        del argv[i]

    return Options(**{k.replace("--", "").replace("-", "_"): v for k, v in args.items()})


def execute(cwd: pathlib.Path, *args: str) -> None:
    r = '", "'.join(args)
    print_fast(f"+\"{r}\"")
    """Simple wrapper to make calling system commands and executables easier"""
    subprocess.check_call(list(args), cwd=f"{cwd}", stdout=sys.stdout, stderr=sys.stderr)


def cmake(cwd: pathlib.Path, *args: str) -> None:
    """Simple wrapper to make calling cmake easier"""
    execute(cwd, which(CMAKE), *args)


include_dirs: List[str] = []
library_dirs: List[str] = []
libraries: List[str] = []
extra_objects: List[str] = []


def pre_build(options: Options) -> None:
    extern_cpp_base: pathlib.Path
    if EXTERN_CPP_SOURCE_SDIST.exists():
        extern_cpp_base = EXTERN_CPP_SOURCE_SDIST
    elif EXTERN_CPP_SOURCE_GIT.exists():
        extern_cpp_base = EXTERN_CPP_SOURCE_GIT
    else:
        raise RuntimeError(
            f"Neither {EXTERN_CPP_SOURCE_SDIST} nor {EXTERN_CPP_SOURCE_GIT} exists. Unable to find "
            f"ifaddrs4cpp sources. Consult the README at "
            f"https://github.com/OddSource/ifaddrs4u/blob/master/ifaddrs4py/README.rst "
            f"for instructions on installing ifaddrs4py."
        )
    print_fast(f"Found ifaddrs4cpp sources at {extern_cpp_base}...")

    cmake_path = extern_cpp_base / CMAKE_BUILD_DIRECTORY
    include_dirs.append(f"{cmake_path / 'include'}")
    library_dirs.append(f"{cmake_path}")
    print_fast(f"Using extra include dirs: {include_dirs}")
    print_fast(f"Using extra library dirs: {library_dirs}")

    static_lib_file = cmake_path / f"libifaddrs4cpp-static.{STATIC_LIBRARY_EXTENSION}"
    extra_objects.append(f"{static_lib_file}")

    test_executable = cmake_path / "ifaddrs4cpp_tests"
    if IS_WINDOWS:
        test_executable = cmake_path / "ifaddrs4cpp_tests.exe"

    if not cmake_path.exists():
        print_fast(f"Creating ifaddrs4cpp build directory {cmake_path}...")
        cmake_path.mkdir(parents=True, exist_ok=True)
    if not static_lib_file.exists() or (options.test_cpp and not test_executable.exists()):
        extra_args: List[str] = []
        if options.test_cpp:
            extra_args.append("-DENABLE_TESTS:BOOL=ON")
        if not options.dynamic:
            extra_args.append("-DBUILD_STATIC_ONLY:BOOL=ON")
        cmake(cmake_path, "-DCMAKE_BUILD_TYPE=Release", "-S", f"{extern_cpp_base}", "-B", f"{cmake_path}", *extra_args)
        cmake(cmake_path, "--build", f"{cmake_path}", "--target", "all", "-j", "14")
        if options.test_cpp:
            exe = "ifaddrs4cpp_tests"
            if IS_WINDOWS:
                exe += ".exe"
            execute(cmake_path, f"{cmake_path / exe}")


if __name__ == "__main__":
    command = sys.argv[1]
    opts = preprocess_options(sys.argv)
    if (
        command in ("build", "install", "test", "pytest", "bdist") or
        command.startswith("build") or command.startswith("install") or command.startswith("bdist")
    ):
        pre_build(opts)

    if command == "sdist":
        pass  # TODO pre_sdist()


cpp_extension = Extension(
    name="ifaddrs4py.extern",
    language="c++",
    sources=[
        "source/cpp/extern.cpp",
        "source/cpp/interface.cpp",
        "source/cpp/version.cpp",
    ],
    include_dirs=include_dirs,
    library_dirs=library_dirs,
    libraries=libraries,
    extra_objects=extra_objects,
    extra_compile_args=["/std:c++17" if IS_WINDOWS else "-std=gnu++17"],
    extra_link_args=[],
    optional=False,
)

setup(
    ext_modules=[
        cpp_extension,
    ],
)
