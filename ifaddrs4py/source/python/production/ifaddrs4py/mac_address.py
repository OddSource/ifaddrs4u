from __future__ import annotations

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


@final
class MacAddress(object):
    __slots__ = ("_formatted", "_data", "_data_length")

    def __init__(self, formatted: str, data: tuple[int, ...]) -> None:
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
