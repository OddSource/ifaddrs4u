from __future__ import annotations

from collections.abc import (
    Callable,
)
from typing import (
    final
)

try:
    from typing import override
except ImportError:
    from typing_extensions import override


__all__ = (
    "MacAddress",
)


get_mac_address_data_from_repr: Callable[[str], tuple[int, ...]] = None
get_mac_address_repr_from_data: Callable[[tuple[int, ...]], str] = None


@final
class MacAddress(object):
    __slots__ = ("_formatted", "_data", "_data_length")

    def __init__(self, formatted: str = None, data: tuple[int, ...] = None) -> None:
        global get_mac_address_repr_from_data, get_mac_address_data_from_repr
        if not formatted and not data:
            raise ValueError("Either 'formatted' or 'data', or both, is required to construct a MacAddress.")
        if (not formatted or not data) and (not get_mac_address_repr_from_data or not get_mac_address_data_from_repr):
            # This is ugly, but in order to avoid circular imports, we don't really have an option.
            from ifaddrs4py.extern import (
                get_mac_address_data_from_repr as _get_mac_address_data_from_repr,
                get_mac_address_repr_from_data as _get_mac_address_repr_from_data,
            )
            get_mac_address_data_from_repr = _get_mac_address_data_from_repr
            get_mac_address_repr_from_data = _get_mac_address_repr_from_data

        if not formatted:
            formatted = get_mac_address_repr_from_data(data)
        elif not data:
            data = get_mac_address_data_from_repr(formatted)

        self._formatted = formatted
        self._data = data
        self._data_length = len(data)

    @property
    def data(self) -> tuple[int, ...]:
        return self._data

    @property
    def data_length(self) -> int:
        return self._data_length

    @override
    def __len__(self) -> int:
        return self._data_length

    @override
    def __str__(self) -> str:
        return self._formatted

    @override
    def __repr__(self) -> str:
        return self.__str__()

    @override
    def __eq__(self, other: MacAddress) -> bool:
        if self._data_length != other._data_length:
            return False

        for i in range(0, self._data_length):
            if self._data[i] != other._data[i]:
                return False

        return True
