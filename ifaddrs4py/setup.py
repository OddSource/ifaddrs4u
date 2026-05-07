#!/usr/bin/env python3
#
# Copyright © 2010-2026 OddSource Code (license@oddsource.io)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import dataclasses
import functools
import os
import pathlib
import shlex
import shutil
import subprocess
import sys
import sysconfig
from typing import (
    Dict,
    List,
    Optional,
)

from setuptools import (  # type: ignore
    Extension,
    setup,
)

UTF_8 = "utf-8"

BASE_PATH = pathlib.Path(__file__).parent.resolve()
EXTERN_CPP_SOURCE_SDIST = BASE_PATH / "extern" / "ifaddrs4cpp"
EXTERN_CPP_SOURCE_GIT = BASE_PATH.parent / "ifaddrs4cpp"
CMAKE_BUILD_DIRECTORY = "cmake-external-build"

CONFIG_RELEASE = "Release"
CONFIG_DEBUG = "Debug"

IS_MACOS = sys.platform == "darwin" or sys.platform == "macos"
IS_WINDOWS = sys.platform == "win32" or sys.platform == "win64" or sys.platform == "cygwin"
IS_CYGWIN = sys.platform == "cygwin"

MACOS_TARGET = "10.15"

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
    except (subprocess.CalledProcessError, FileNotFoundError):
        WHICH_COMMAND = ["which"]


def print_fast(string):
    sys.stdout.write(f"{string}\n")
    sys.stdout.flush()


@functools.lru_cache(maxsize=128)
def which(executable: str) -> str:
    """For determining full paths to necessary system commands and installed executables"""
    try:
        value = subprocess.check_output(WHICH_COMMAND + [executable], encoding=UTF_8). \
                           strip().splitlines()[0].strip()
        print_fast(f"Using {executable} at {value}...")
        return value
    except (subprocess.CalledProcessError, FileNotFoundError) as e:
        raise RuntimeError(
            f"Required executable `{executable}` not found on this system. Please install it before building "
            f"ifaddrs4cpp and ifaddrs4py. Error detail: {e}"
        )


@dataclasses.dataclass
class Options:
    clean: bool = False
    cpp_asan: bool = False
    cpp_debug: bool = False
    cpp_test: bool = False
    dynamic: bool = False


def preprocess_options(argv: List[str]) -> Options:
    """For extracting custom command-line arguments from the setup command"""
    args = {
        "--clean": False,
        "--cpp-asan": False,
        "--cpp-debug": False,
        "--cpp-test": False,
        "--dynamic": False,
    }

    to_del: List[int] = []
    for i in range(1, len(argv)):
        if argv[i] in args:
            print_fast(f"With {argv[i]}")
            args[argv[i]] = True
            to_del.append(i)
    for i in reversed(to_del):
        del argv[i]

    o = Options(**{k.replace("--", "").replace("-", "_"): v for k, v in args.items()})

    if o.cpp_asan:
        if IS_WINDOWS:
            raise ValueError("Option --cpp-asan is not supported on Windows platforms")
        if not o.cpp_debug:
            raise ValueError("Option --cpp-asan requires option --cpp-debug")

    return o


def execute(cwd: pathlib.Path, *args: str, extra_env: Optional[Dict[str, str]] = None) -> None:
    """Simple wrapper to make calling system commands and executables easier"""
    r = '", "'.join(args)
    print_fast(f"+ \"{r}\"")

    env: Optional[Dict[str, str]] = None
    if extra_env:
        env = {**os.environ, **extra_env}

    subprocess.check_call(list(args), cwd=f"{cwd}", stdout=sys.stdout, stderr=sys.stderr, env=env)


def cmake(cwd: pathlib.Path, *args: str) -> None:
    """Simple wrapper to make calling cmake easier"""
    execute(cwd, which(CMAKE), *args)


def ctest(cwd: pathlib.Path, *args: str) -> None:
    """Simple wrapper to make calling ctest easier"""
    execute(cwd, which(CTEST), *args)


include_dirs: List[str] = []
library_dirs: List[str] = []
libraries: List[str] = []
extra_objects: List[str] = []
extra_compile_args: List[str] = []
extra_link_args: List[str] = []
define_macros: List[tuple[str, str]] = [("IFADDRS4CPP_INLINE_SOURCE", "1")]
undef_macros: List[str] = []
debug = "no"
if IS_WINDOWS:
    extra_compile_args.extend(["/std:c++17", "/wd4275", "/wd4251", "/wd4455"])
else:
    extra_compile_args.extend([
        "-std=c++17",
        "-fvisibility=hidden",
        "-Wall",
        "-Wdeprecated",
        "-Wextra",
        "-Wpedantic",
        "-Wshadow",
        "-Wunused",
        "-Werror"
    ])
    if IS_MACOS:
        # noinspection PyDeprecation
        target = (
            sysconfig.get_config_var("MACOSX_DEPLOYMENT_TARGET") or
            sysconfig.get_config_var("MACOS_DEPLOYMENT_TARGET")
        )
        if not target or tuple(int(i) for i in target.split(".")) < (10, 15):
            print_fast(f"Using MACOSX_DEPLOYMENT_TARGET = {MACOS_TARGET}, currently {target}")
            os.environ["MACOSX_DEPLOYMENT_TARGET"] = MACOS_TARGET
            os.environ["MACOS_DEPLOYMENT_TARGET"] = MACOS_TARGET
        target_opts = [f"-mmacosx-version-min={MACOS_TARGET}"]
        extra_compile_args.extend(target_opts)
        extra_link_args.extend(target_opts)


def pre_build(options: Options) -> None:
    global debug
    if not IS_WINDOWS and not IS_CYGWIN:
        if options.cpp_debug:
            debug = "yes"
            extra_compile_args.extend(["-g3", "-O0"])
        else:
            define_macros.append(("NDEBUG", "1"))
            extra_compile_args.extend(["-g", "-O3"])

        # The "Unix" compiler in setuptools blindly takes the value from CFLAGS and uses it for the C++ compiler,
        # without any filtering, ignoring the value passed to the debug option. But the environment variable CXXFLAGS
        # can override this. So we set CXXFLAGS to a filtered subset of CFLAGS so that we, exclusively, can control
        # debug and optimization options.
        # noinspection PyDeprecation
        original_cf_flags = sysconfig.get_config_var("CFLAGS")
        new_cf_flags = shlex.join([
            f for f in shlex.split(original_cf_flags)
            if "NDEBUG" not in f and f != "-g" and f != "--optimize" and not f.startswith("-O") and (
                    f == "-Wunreachable-code" or not f.startswith("-W")
            )
        ])
        print_fast(f"Setting CXXFLAGS to '{new_cf_flags}' from original CFLAGS '{original_cf_flags}'")
        os.environ["CXXFLAGS"] = new_cf_flags

    print_fast(f"debug = {debug}")

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
    include_dirs.append(f"{extern_cpp_base / 'include'}")
    include_dirs.append(f"{cmake_path / 'include'}")
    #library_dirs.append(f"{cmake_path}")
    print_fast(f"Using extra include dirs: {include_dirs}")
    #print_fast(f"Using extra library dirs: {library_dirs}")

    if options.clean:
        for d in (cmake_path, BASE_PATH / "build", BASE_PATH / "dist"):
            if d.exists():
                print_fast(f"Cleaning {d}")
                shutil.rmtree(d, ignore_errors=True)

    config = CONFIG_DEBUG if options.cpp_debug else CONFIG_RELEASE
    suffix = "-d" if options.cpp_debug else ""

    asan_options: List[str] = []
    if options.cpp_asan:
        asan_options.extend(["-fsanitize=address", "-fsanitize-address-use-after-scope"])
        if IS_MACOS:
            asan_options.extend(["-fsanitize-address-use-odr-indicator", "-shared-libsan"])
        extra_compile_args.extend(asan_options)
        extra_link_args.extend(asan_options)

    static_lib_file: pathlib.Path
    test_executable: pathlib.Path
    if IS_WINDOWS:
        static_lib_file = cmake_path / config / f"ifaddrs4cpp-static{suffix}.{STATIC_LIBRARY_EXTENSION}"
        test_executable = cmake_path / config / "ifaddrs4cpp_tests.exe"
    else:
        static_lib_file = cmake_path / f"libifaddrs4cpp-static{suffix}.{STATIC_LIBRARY_EXTENSION}"
        test_executable = cmake_path / "ifaddrs4cpp_tests"
    #extra_objects.append(f"{static_lib_file}")

    if not static_lib_file.exists() or (options.cpp_test and not test_executable.exists()):
        extra_args: List[str] = []
        if options.cpp_test:
            extra_args.extend(["-DENABLE_TESTS:BOOL=ON", "-DSKIP_BOOST:BOOL=ON"])
        if options.cpp_asan:
            extra_args.extend(["-DENABLE_ADDRESS_SANITIZER:BOOL=ON", "-DSHARED_ADDRESS_SANITIZER:BOOL=ON"])
        if not options.dynamic:
            extra_args.append("-DBUILD_STATIC_ONLY:BOOL=ON")
        cmake(
            extern_cpp_base,
            f"-DCMAKE_BUILD_TYPE={config}",
            "-S", f"{extern_cpp_base}",
            "-B", f"{cmake_path}",
            *extra_args
        )
        cmake(cmake_path, "--build", f"{cmake_path}", "--config", config, "-j", "14")
        if options.cpp_test:
            if options.cpp_asan:
                extra_env: Dict[str, str] = {}
                if IS_MACOS:
                    extra_env["ASAN_OPTIONS"] = "detect_stack_use_after_return=1:verify_asan_link_order=0"
                else:
                    extra_env["ASAN_OPTIONS"] = \
                        "detect_stack_use_after_return=1:detect_leaks=1:verify_asan_link_order=0"
                execute(cmake_path, f"{test_executable}", extra_env=extra_env)
            else:
                ctest(
                    cmake_path,
                    "--build-config", config,
                    "--verbose",
                    "--test-action", "Test",
                    "--output-on-failure",
                )


def pre_sdist() -> None:
    if not EXTERN_CPP_SOURCE_SDIST.exists():
        if not EXTERN_CPP_SOURCE_GIT.exists():
            raise RuntimeError(
                f"Directory {EXTERN_CPP_SOURCE_GIT} does not exist and external sources are not pre-copied, so the "
                f"source distribution cannot be packaged. Without some juggling, the source distribution can be "
                f"packaged only from a Git clone of the project."
            )
        EXTERN_CPP_SOURCE_SDIST.mkdir(parents=True, exist_ok=True)

    tests_src = EXTERN_CPP_SOURCE_GIT / "tests"
    tests_dest = EXTERN_CPP_SOURCE_SDIST / "tests"
    if not tests_dest.exists():
        tests_dest.mkdir()

    print_fast(f"Temporarily copying files from {EXTERN_CPP_SOURCE_GIT} to {EXTERN_CPP_SOURCE_SDIST}...")
    for file in [EXTERN_CPP_SOURCE_GIT / "CMakeLists.txt", ] + \
            list(EXTERN_CPP_SOURCE_GIT.glob("**/*.h")) + \
            list(EXTERN_CPP_SOURCE_GIT.glob("**/*.hpp")) + \
            list(EXTERN_CPP_SOURCE_GIT.glob("**/*.ipp")) + \
            list(EXTERN_CPP_SOURCE_GIT.glob("**/*.cpp")):
        shutil.copy(file, EXTERN_CPP_SOURCE_SDIST)

    print_fast(f"Temporarily copying files from {tests_src} to {tests_dest}...")
    for file in tests_src.glob("*"):
        shutil.copy(file, tests_dest)


def post_sdist() -> None:
    if EXTERN_CPP_SOURCE_SDIST.parent.exists():
        print_fast(f"Removing temporary directory {EXTERN_CPP_SOURCE_SDIST.parent}")
        shutil.rmtree(EXTERN_CPP_SOURCE_SDIST.parent, ignore_errors=True)


if __name__ == "__main__":
    command = sys.argv[1]
    opts = preprocess_options(sys.argv)
    if (
        command in ("build", "install", "test", "pytest", "bdist") or
        command.startswith("build") or command.startswith("install") or command.startswith("bdist")
    ):
        pre_build(opts)

    if command == "sdist":
        pre_sdist()


cpp_extension = Extension(
    name="ifaddrs4py.extern",
    language="c++",
    sources=[
        "source/cpp/extern.cpp",
        "source/cpp/helpers.cpp",
        "source/cpp/interface.cpp",
        "source/cpp/ip_address.cpp",
        "source/cpp/mac_address.cpp",
        "source/cpp/version.cpp",
    ],
    include_dirs=include_dirs,
    library_dirs=library_dirs,
    libraries=libraries,
    extra_objects=extra_objects,
    extra_compile_args=extra_compile_args,
    extra_link_args=extra_link_args,
    define_macros=define_macros,
    undef_macros=undef_macros,
    optional=False,
)

setup(
    ext_modules=[
        cpp_extension,
    ],
    options={"build_ext": {"debug": debug, "parallel": "15"}},
)

if __name__ == "__main__":
    command = sys.argv[1]
    if command == "sdist":
        post_sdist()
