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

from ipaddress import (
    AddressValueError,
    IPv4Address,
    IPv6Address as _IPv6Address,
)
import sys
from typing import (
    Any,
    Optional,
)

if sys.version_info >= (3, 9):
    Tuple = tuple
else:
    from typing import Tuple

if sys.version_info >= (3, 12):
    from typing import override
else:
    from typing_extensions import override

__all__ = (
    "IPv4Address",
    "IPv6Address",
)

_base_has_scope: bool
_slots: Tuple[str, ...]
if hasattr(_IPv6Address("::1"), "scope_id"):
    _base_has_scope = True
    _slots = ("_scope_number", )
else:
    _base_has_scope = False
    _slots = ("_scope_id", "_scope_number")


class IPv6Address(_IPv6Address):
    """
    This extension class is three-fold: First, it makes it easier to construct an IPv6 address
    from packed bytes while also specifying a scope ID. Second, it accounts for the fact that
    "older" Python versions don't support scope IDs at all. And third, it allows us to store
    both the scope name *and* the scope ID.
    """
    __slots__ = _slots

    def __init__(self, address: Any, scope_id: Optional[str] = None, scope_number: Optional[int] = None) -> None:
        self._scope_id: Optional[str] = None
        self._scope_number: Optional[int] = scope_number

        if isinstance(address, (bytes, int)):
            super().__init__(address)
            if scope_id:
                self._scope_id = scope_id
        else:
            addr_str = str(address)
            scope_id_: Optional[str] = None
            scope_number_: Optional[int] = None
            if "%" in addr_str:
                addr_str, scope_id_, scope_number_ = self._split_scope_id_and_number(addr_str)
            super().__init__(addr_str)
            self._scope_id = scope_id or scope_id_
            self._scope_number = scope_number or scope_number_

        if not self._scope_id and self._scope_number:
            self._scope_id = str(self._scope_number)
        if not self._scope_number and self._scope_id and self._scope_id.isnumeric():
            self._scope_number = int(self._scope_id)

    if not _base_has_scope:
        @override
        def __str__(self):
            ip_str = super().__str__()
            return ip_str + '%' + self._scope_id if self._scope_id else ip_str

        @override
        def __hash__(self):
            return hash((self._ip, self._scope_id))

        @override
        def __eq__(self, other):
            address_equal = super().__eq__(other)
            if address_equal is NotImplemented:
                return NotImplemented
            if not address_equal:
                return False
            return self._scope_id == getattr(other, '_scope_id', None)

        @property
        def scope_id(self) -> Optional[str]:
            return self._scope_id

    @property
    def scope_number(self) -> Optional[int]:
        return self._scope_number

    @staticmethod
    def _split_scope_id_and_number(ip_str) -> Tuple[str, Optional[str], Optional[int]]:
        scope_number: Optional[int] = None
        addr, sep, scope_id = ip_str.partition("%")
        if not sep:
            scope_id = None
        elif not scope_id or "%" in scope_id:
            raise AddressValueError('Invalid IPv6 address: "%r"' % ip_str)
        elif scope_id.isnumeric():
            scope_number = int(scope_id)
            scope_id = None
        return addr, scope_id, scope_number
