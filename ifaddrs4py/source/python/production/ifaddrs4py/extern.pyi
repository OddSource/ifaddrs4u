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

import enum
import sys
from typing import (
    Optional,
    Union,
)

if sys.version_info >= (3, 9):
    from collections.abc import (
        Callable,
        Iterator,
    )
    Tuple = tuple
else:
    from typing import (
        Callable,
        Iterator,
        Tuple,
    )

from ifaddrs4py.interface import (
    Interface,
    InterfaceIPAddress,
)
from ifaddrs4py.ip_address import (
    IPv4Address,
    IPv6Address,
)
from ifaddrs4py.mac_address import MacAddress


__VERSION_EXT__: str
__VERSION_INFO_EXT__: Tuple[Union[str, int], ...]

_TEST_LOOPBACK_V4: IPv4Address
_TEST_LOOPBACK_V6: IPv6Address
_TEST_MAC_ADDRESS: MacAddress


def get_sample_interface_ipv4_address() -> InterfaceIPAddress[IPv4Address]: ...


def get_sample_interface_ipv6_address() -> InterfaceIPAddress[IPv6Address]: ...


def get_sample_interface_scoped_ipv6_address() -> InterfaceIPAddress[IPv6Address]: ...


def get_sample_interface() -> Interface: ...


def get_mac_address_data_from_repr(s: str) -> Tuple[int, ...]: ...


def get_mac_address_repr_from_data(d: Tuple[int, ...]) -> str: ...


class InterfaceIPAddressFlag(enum.IntEnum):
    Anycast: int
    AutoConfigured: int
    Deprecated: int
    Detached: int
    Duplicated: int
    Dynamic: int
    Optimistic: int
    NoDad: int
    Secured: int
    Temporary: int
    Tentative: int


class InterfaceFlag(enum.IntEnum):
    BroadcastAddressSet: int
    DebugEnabled: int
    IsLoopback: int
    IsPointToPoint: int
    IsRunning: int
    IsUp: int
    NoARP: int
    PromiscuousModeEnabled: int
    ReceiveAllMulticastPackets: int
    SupportsMulticast: int

    # The following members may not be present on all platforms
    Master: int
    Simplex: int
    Slave: int
    Smart: int
    TransmissionInProgress: int


class IllegalStateError(RuntimeError): ...


class InterfaceBrowser:
    def get_interfaces(self) -> Tuple[Interface, ...]: ...

    def length(self) -> int: ...

    def __len__(self) -> int: ...

    def get_interface(self, key: Union[str, int]) -> Optional[Interface]: ...

    def __getitem__(self, key: Union[str, int]) -> Interface: ...

    def for_each_interface(self, do_this: Callable[[Interface], bool]) -> bool: ...

    def __iter__(self) -> Iterator[Interface]: ...

    def __repr__(self) -> str: ...

    def __str__(self) -> str: ...
