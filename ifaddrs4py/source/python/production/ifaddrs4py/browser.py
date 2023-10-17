import sys

from typing import (
    final,
    Union,
    Tuple,
)

if sys.version_info.minor < 9:
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

try:
    from typing import override
except ImportError:
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
    __slots__ = ("_extern", )

    def __init__(self) -> None:
        self._extern = Extern_InterfaceBrowser()

    def get_interface(self, index_or_name: Union[str, int]) -> Interface:
        return self._extern.get_interface(index_or_name)

    def get_interfaces(self) -> Tuple[Interface, ...]:
        return self._extern.get_interfaces()

    def for_each_interface(self, do_this: Callable[[Interface], bool]) -> bool:
        return self._extern.for_each_interface(do_this)

    def interfaces(self) -> Generator[Interface, None, None]:
        for interface in self:
            yield interface

    def __len__(self) -> int:
        return self._extern.__len__()

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

    @classmethod
    @property
    def version(unused) -> str:
        return __VERSION__
