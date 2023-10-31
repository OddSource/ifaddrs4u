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

import sys

__all__ = (
    "IS_MACOS",
    "IS_WINDOWS",
)

IS_MACOS = sys.platform == "darwin" or sys.platform == "macos"
IS_WINDOWS = sys.platform == "win32" or sys.platform == "win64" or sys.platform == "cygwin"
