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

from typing import (
    final,
    Optional,
    Union,
    Tuple,
)

if sys.version_info <= (3, 9):
    from typing import (
        Callable,
        Generator,
        Iterator,
    )
else:
    from collections.abc import (
        Callable,
        Generator,
        Iterator,
    )

if sys.version_info >= (3, 12):
    from typing import override
else:
    from typing_extensions import override

from ifaddrs4py.extern import (
    IllegalStateError,
    InterfaceBrowser as Extern_InterfaceBrowser,
)
from ifaddrs4py.interface import Interface
from ifaddrs4py.version import __VERSION__


__all__ = (
    "IllegalStateError",
    "InterfaceBrowser",
)


@final
class InterfaceBrowser:
    version = __VERSION__

    __slots__ = ("_extern", )

    def __init__(self) -> None:
        self._extern = Extern_InterfaceBrowser()

    def get_interface(self, index_or_name: Union[str, int]) -> Optional[Interface]:
        try:
            return self._extern.get_interface(index_or_name)
        except KeyError:
            return None

    def get_interfaces(self) -> Tuple[Interface, ...]:
        return self._extern.get_interfaces()

    def for_each_interface(self, do_this: Callable[[Interface], bool]) -> bool:
        return self._extern.for_each_interface(do_this)

    def interfaces(self) -> Generator[Interface, None, None]:
        for interface in self:
            yield interface

    def __len__(self) -> int:
        return self._extern.__len__()

    def length(self) -> int:
        return self._extern.length()

    def __getitem__(self, index_or_name: Union[str, int]) -> Interface:
        return self._extern.__getitem__(index_or_name)

    def __iter__(self) -> Iterator[Interface]:
        return self._extern.__iter__()

    @override
    def __repr__(self) -> str:
        return self._extern.__repr__()

    @override
    def __str__(self) -> str:
        return self._extern.__str__()
