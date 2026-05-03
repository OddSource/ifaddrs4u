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

from ipaddress import (
    AddressValueError,
    IPv4Address,
    IPv6Address as _IPv6Address,
)
from typing import (
    Any,
    Optional,
)

__all__ = (
    "IPv4Address",
    "IPv6Address",
)


class IPv6Address(_IPv6Address):
    """
    This extension class is three-fold: First, it makes it easier to construct an IPv6 address
    from packed bytes while also specifying a scope ID. Second, it accounts for the fact that
    "older" Python versions don't support scope IDs at all. And third, it allows us to store
    both the scope name *and* the scope ID.
    """
    __slots__ = ("_scope_number", )

    def __init__(self, address: Any, scope_id: Optional[str] = None, scope_number: Optional[int] = None) -> None:
        self._scope_number: Optional[int] = scope_number

        super().__init__(address)
        if isinstance(address, (bytes, int)) and scope_id:
            self._scope_id = scope_id
        elif scope_id:
            if self._scope_id:
                if scope_id != self._scope_id:
                    raise AddressValueError(
                        f"In IPv6 address {address}, included scope ID {self._scope_id} does not match explicit "
                        f"scope ID {scope_id}."
                    )
            else:
                self._scope_id = scope_id

        if self._scope_id and self._scope_id.isnumeric():
            self._scope_number = int(self._scope_id)

        if scope_number:
            if self._scope_number:
                if scope_number != self._scope_number:
                    raise AddressValueError(
                        f"In IPv6 address {address}, included scope number {self._scope_number} does not match "
                        f"explicit scope number {scope_number}."
                    )
            else:
                self._scope_number = scope_number
            if not self._scope_id:
                self._scope_id = str(self._scope_number)

    @property
    def scope_number(self) -> Optional[int]:
        return self._scope_number
