#
# Copyright © 2010-2023 OddSource Code (license@oddsource.io)
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

try:
    from ifaddrs4py.extern import (
        __VERSION_EXT__,
        __VERSION_INFO_EXT__,
    )
    _skip_version_check = False
except ImportError:
    __VERSION_EXT__ = "unknown"
    __VERSION_INFO_EXT__ = ()
    _skip_version_check = True

__VERSION__ = "0.1.0-alpha0"
__VERSION_INFO__ = (0, 1, 0, "alpha0")

__all__ = (
    "__VERSION__",
    "__VERSION_EXT__",
    "__VERSION_INFO__",
    "__VERSION_INFO_EXT__",
)

if not _skip_version_check:
    import warnings

    class VersionMismatchWarning(UserWarning):
        pass

    if __VERSION__ != __VERSION_EXT__:
        warnings.warn(
            f"ifaddrs4py version: C++ extension version {__VERSION_EXT__} "
            f"does not match Python library version {__VERSION__}",
            category=VersionMismatchWarning,
            stacklevel=1,
        )
    elif __VERSION_INFO__ != __VERSION_INFO_EXT__:
        warnings.warn(
            f"ifaddrs4py version: C++ extension version info {__VERSION_INFO__} "
            f"does not match Python library version info {__VERSION_INFO_EXT__}",
            category=VersionMismatchWarning,
            stacklevel=1,
        )
