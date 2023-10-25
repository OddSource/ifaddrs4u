#!/usr/bin/env python3

"""
Run this script *after* a release in order to prepare for the next version of development. It will traverse
all of the subprojects and increment their version information so that everything matches. It is recommended
that you run with --dry-run first, check the intended changes for sanity, and then run without --dry-run.
"""

import argparse
import dataclasses
import difflib
import pathlib
import re
import sys
import traceback
from typing import (
    Optional,
)


VERSION_RE = re.compile(r"^[0-9]+\.[0-9]+\.[0-9]+$")
SUFFIX_RE = re.compile(r"^[a-z][a-z0-9]{1,9}$")

UTF_8 = "utf-8"
ASCII = "ascii"
ISO_1 = "iso-8859-1"
ISO_2 = "iso-8859-2"

PATH_BASE = pathlib.Path(__file__).parent.resolve()
PATH_CPP = PATH_BASE / "ifaddrs4cpp"
PATH_J = PATH_BASE / "ifaddrs4j"
PATH_PL = PATH_BASE / "ifaddrs4pl"
PATH_PY = PATH_BASE / "ifaddrs4py"

PATH_CPP_CMakeLists_TXT = PATH_CPP / "CMakeLists.txt"

PATH_J_VERSION_PROPERTIES = PATH_J / "source" / "production" / "resources" / "io" / "oddsource" / "java" / "net" / "ifaddrs4j" / "version.properties"
PATH_J_POM_XML = PATH_J / "pom.xml"

PATH_PY_VERSION_PY = PATH_PY / "source" / "python" / "production" / "ifaddrs4py" / "version.py"


@dataclasses.dataclass
class VersionInfo:
    version: str
    version_with_suffix: str
    major: int
    minor: int
    patch: int
    suffix: Optional[int]


def diff(a: str, b: str, file: pathlib.Path) -> None:
    f=f"{file}"
    sys.stdout.writelines(difflib.unified_diff(
        a=a.splitlines(keepends=True),
        b=b.splitlines(keepends=True),
        fromfile=f,
        tofile=f,
    ))
    sys.stdout.flush()


def read_text_and_encoding(file: pathlib.Path) -> bool:
    first_exc_info = None
    for encoding in (UTF_8, ASCII, ISO_1, ISO_2):
        try:
            text = file.read_text(encoding=encoding)
            return text, (encoding if encoding in (UTF_8, ISO_1) else UTF_8)
        except ValueError:
            if not first_exc_info:
                first_exc_info = sys.exc_info()
    raise first_exc_info[1].with_traceback(first_exc_info[2])


def update_cpp_cmake_file(info: VersionInfo, dry_run: bool) -> bool:
    original_text, encoding = read_text_and_encoding(PATH_CPP_CMakeLists_TXT)
    new_text = ""
    version_found = suffix_found = False
    for line in original_text.splitlines():
        if line.startswith("        VERSION "):
            new_text += f"        VERSION {info.version}\n"
            version_found = True
        elif line.startswith('set(PROJECT_VERSION_SUFFIX "'):
            new_text += f'set(PROJECT_VERSION_SUFFIX "{info.suffix[1:]}")\n'
            suffix_found = True
        else:
            new_text += f"{line}\n"

    diff(original_text, new_text, PATH_CPP_CMakeLists_TXT)

    if not version_found:
        print(f"Version property not replaced in {PATH_CPP_CMakeLists_TXT}")
    if not suffix_found:
        print(f"Suffix property not replaced in {PATH_CPP_CMakeLists_TXT}")
    if not version_found or not suffix_found:
        return False

    if not dry_run:
        PATH_CPP_CMakeLists_TXT.write_text(new_text, encoding=encoding)
        print(f"UPDATED SUCCESSFULLY ({encoding}): {PATH_CPP_CMakeLists_TXT}\n")
    else:
        print(f"DRY RUN, did not update {PATH_CPP_CMakeLists_TXT} ({encoding})\n")

    return True


def update_java_properties(info: VersionInfo, dry_run: bool) -> bool:
    original_text, encoding = read_text_and_encoding(PATH_J_VERSION_PROPERTIES)
    new_text = ""
    lines_replaced = 0
    for line in original_text.splitlines():
        if line.startswith("ifaddrs4j.version="):
            new_text += f"ifaddrs4j.version={info.version_with_suffix}\n"
            lines_replaced += 1
        elif line.startswith("ifaddrs4j.version.",):
            attr, extra = line.replace("ifaddrs4j.version.", "").split("=", maxsplit=1)
            new_text += f"ifaddrs4j.version.{attr}={getattr(info, attr)}\n"
            lines_replaced += 1
        else:
            new_text += f"{line}\n"

    diff(original_text, new_text, PATH_J_VERSION_PROPERTIES)

    if lines_replaced < 5:
        print(f"Not enough properties replaced in {PATH_J_VERSION_PROPERTIES} (expected 5, replaced {lines_replaced})")
        return False

    if not dry_run:
        PATH_J_VERSION_PROPERTIES.write_text(new_text, encoding=encoding)
        print(f"UPDATED SUCCESSFULLY ({encoding}): {PATH_J_VERSION_PROPERTIES}\n")
    else:
        print(f"DRY RUN, did not update {PATH_J_VERSION_PROPERTIES} ({encoding})\n")

    return True


def update_java_pom(info: VersionInfo, dry_run: bool) -> bool:
    original_text, encoding = read_text_and_encoding(PATH_J_POM_XML)
    new_text = ""
    version_found = False
    for line in original_text.splitlines():
        if line.startswith("    <version>"):
            new_text += f"    <version>{info.version_with_suffix}-SNAPSHOT</version>\n"
            version_found = True
        else:
            new_text += f"{line}\n"

    diff(original_text, new_text, PATH_J_POM_XML)

    if not version_found:
        print(f"Version not replaced in {PATH_J_POM_XML}")
        return False

    if not dry_run:
        PATH_J_POM_XML.write_text(new_text, encoding=encoding)
        print(f"UPDATED SUCCESSFULLY ({encoding}): {PATH_J_POM_XML}\n")
    else:
        print(f"DRY RUN, did not update {PATH_J_POM_XML} ({encoding})\n")

    return True


def update_python_version_py(info: VersionInfo, dry_run: bool) -> bool:
    original_text, encoding = read_text_and_encoding(PATH_PY_VERSION_PY)
    new_text = ""
    version_found = version_info_found = False
    for line in original_text.splitlines():
        if line.startswith('__VERSION__ = "'):
            new_text += f'__VERSION__ = "{info.version_with_suffix}"\n'
            version_found = True
        elif line.startswith("__VERSION_INFO__ = ("):
            new_text += f"__VERSION_INFO__ = ({info.major}, {info.minor}, {info.patch}"
            if info.suffix:
                new_text += f', "{info.suffix[1:]}"'
            new_text += f")\n"
            version_info_found = True
        else:
            new_text += f"{line}\n"

    diff(original_text, new_text, PATH_PY_VERSION_PY)

    if not version_found:
        print(f"__VERSION__ not replaced in {PATH_PY_VERSION_PY}")
    if not version_info_found:
        print(f"__VERSION_INFO__ not replaced in {PATH_PY_VERSION_PY}")
    if not version_found or not version_info_found:
        return False

    if not dry_run:
        PATH_PY_VERSION_PY.write_text(new_text, encoding=encoding)
        print(f"UPDATED SUCCESSFULLY ({encoding}): {PATH_PY_VERSION_PY}\n")
    else:
        print(f"DRY RUN, did not update {PATH_PY_VERSION_PY} ({encoding})\n")

    return True


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("new_version", help="The new version to use")
    parser.add_argument("--dry-run", action="store_true", default=False,
                        help="Display the changes to be made, but don't actually make them")
    args = parser.parse_args()

    version: str
    suffix: Optional[str]
    version, suffix = args.new_version, None
    if "-" in args.new_version:
        version, suffix = args.new_version.split('-', maxsplit=1)

    if not VERSION_RE.fullmatch(version) or (suffix and not SUFFIX_RE.fullmatch(suffix)):
        parser.error(
            "The version must be in format MAJOR.MINOR.PATCH[-SUFFIX], where all parts except the suffix are "
            "required; major, minor, and patch must be numeric only; and the suffix must start with lowercase "
            "a-z, contain only characters a-z and 0-9, and be at most 10 characters long."
        )

    major, minor, patch = version.split(".", maxsplit=2)
    if suffix:
        suffix = f"-{suffix}"

    info = VersionInfo(version, args.new_version, int(major), int(minor), int(patch), suffix)

    for task in (update_cpp_cmake_file, update_java_properties, update_java_pom, update_python_version_py):
        try:
            if not task(info, args.dry_run):
                print("\nUpdate failed. You may need to undo changes with Git.")
                return 2
        except Exception as e:
            traceback.print_exc()
            print("\nUpdate failed. You may need to undo changes with Git.")
            return 3


if __name__ == "__main__":
    sys.exit(main())
