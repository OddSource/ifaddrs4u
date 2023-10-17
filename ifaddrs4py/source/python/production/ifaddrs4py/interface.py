from __future__ import annotations

import ipaddress
from typing import (
    final,
    Generic,
    Optional,
    TypeVar,
)
import uuid

try:
    from typing import override
except ImportError:
    from typing_extensions import override

from ifaddrs4py.constants import (
    IS_WINDOWS,
)
from ifaddrs4py.extern import (
    InterfaceIPAddressFlag,
    InterfaceFlag,
)
from ifaddrs4py.mac_address import (
    MacAddress,
)

__all__ = (
    "IPAddressT",
    "Interface",
    "InterfaceFlag",
    "InterfaceIPAddress",
    "InterfaceIPAddressFlag",
)

IPAddressT = TypeVar("IPAddressT", ipaddress.IPv4Address, ipaddress.IPv6Address)

_IP_ADDRESS_FLAG_DISPLAYS = {
    "autoconf": InterfaceIPAddressFlag.AutoConfigured,
    "deprecated": InterfaceIPAddressFlag.Deprecated,
    "secured": InterfaceIPAddressFlag.Secured,
    "temporary": InterfaceIPAddressFlag.Temporary,
    "anycast": InterfaceIPAddressFlag.Anycast,
    "detached": InterfaceIPAddressFlag.Detached,
    "duplicated": InterfaceIPAddressFlag.Duplicated,
    "dynamic": InterfaceIPAddressFlag.Dynamic,
    "optimistic": InterfaceIPAddressFlag.Optimistic,
    "tentative": InterfaceIPAddressFlag.Tentative,
    "nodad": InterfaceIPAddressFlag.NoDad,
}

_INTERFACE_FLAG_DISPLAYS = {
    "UP": InterfaceFlag.IsUp,
    "RUNNING": InterfaceFlag.IsRunning,
    "LOOPBACK": InterfaceFlag.IsLoopback,
    "POINTOPOINT": InterfaceFlag.IsPointToPoint,
    "BROADCAST": InterfaceFlag.BroadcastAddressSet,
    "MULTICAST": InterfaceFlag.SupportsMulticast,
    "DEBUG": InterfaceFlag.DebugEnabled,
    "PROMISC": InterfaceFlag.PromiscuousModeEnabled,
    "ALLMULTI": InterfaceFlag.ReceiveAllMulticastPackets,
    "NOARP": InterfaceFlag.NoARP,
    "SMART": InterfaceFlag.NoTrailers,
}
if hasattr(InterfaceFlag, "TransmissionInProgress"):
    # noinspection PyUnresolvedReferences
    _INTERFACE_FLAG_DISPLAYS["OACTIVE"] = InterfaceFlag.TransmissionInProgress
if hasattr(InterfaceFlag, "Simplex"):
    # noinspection PyUnresolvedReferences
    _INTERFACE_FLAG_DISPLAYS["SIMPLEX"] = InterfaceFlag.Simplex
if hasattr(InterfaceFlag, "Master"):
    # noinspection PyUnresolvedReferences
    _INTERFACE_FLAG_DISPLAYS["MASTER"] = InterfaceFlag.Master
if hasattr(InterfaceFlag, "Slave"):
    # noinspection PyUnresolvedReferences
    _INTERFACE_FLAG_DISPLAYS["SLAVE"] = InterfaceFlag.Slave


@final
class InterfaceIPAddress(Generic[IPAddressT]):
    __slots__ = ("_address", "_flags", "_prefix_length", "_broadcast_address", "_point_to_point_destination")

    def __init__(
        self,
        address: IPAddressT,
        flags: int,
        prefix_length: int,
        broadcast_address: Optional[IPAddressT] = None,
        point_to_point_destination: Optional[IPAddressT] = None,
    ) -> None:
        self._address = address
        self._flags = flags
        self._prefix_length = prefix_length
        self._broadcast_address = broadcast_address
        self._point_to_point_destination = point_to_point_destination

    @property
    def address(self) -> IPAddressT:
        return self._address

    @property
    def prefix_length(self) -> int:
        return self._prefix_length

    @property
    def broadcast_address(self) -> Optional[IPAddressT]:
        return self._broadcast_address

    @property
    def point_to_point_destination(self) -> Optional[IPAddressT]:
        return self._point_to_point_destination

    def is_flag_enabled(self, flag: InterfaceIPAddressFlag) -> bool:
        return (self._flags & flag.value) == flag.value

    @override
    def __str__(self) -> str:
        string = f"{self._address}"
        if self._prefix_length > 0:
            string += f"/{self._prefix_length}"
        if self._broadcast_address is not None:
            string += f" broadcast {self._broadcast_address}"
        elif self._point_to_point_destination is not None:
            string += f" destination {self._point_to_point_destination}"

        if self._flags:
            for display, flag in _IP_ADDRESS_FLAG_DISPLAYS.items():
                if (self._flags & flag.value) == flag.value:
                    string += f" {display}"

        if getattr(self._address, "scope_id", None):
            string += f" scopeid {self._address.scope_id}"

        return string

    @override
    def __repr__(self) -> str:
        return self.__str__()

    @override
    def __eq__(self, other: InterfaceIPAddress[IPAddressT]) -> str:
        return (
            self._flags == other._flags and
            self._prefix_length == other._prefix_length and
            self._address == other._address and
            self._broadcast_address == other._broadcast_address and
            self._point_to_point_destination == other._point_to_point_destination
        )


@final
class Interface(object):
    if IS_WINDOWS:
        __slots__ = (
            "_index", "_name", "_windows_uuid", "_flags", "_mtu",
            "_mac_address", "_ipv4_addresses", "_ipv6_addresses",
        )

        def __init__(
            self,
            index: int,
            name: str,
            windows_uuid: str,
            flags: int,
            mtu: Optional[int] = None,
            mac_address: Optional[MacAddress] = None,
            ipv4_addresses: tuple[InterfaceIPAddress[ipaddress.IPv4Address], ...] = None,
            ipv6_addresses: tuple[InterfaceIPAddress[ipaddress.IPv6Address], ...] = None,
        ) -> None:
            self._index = index
            self._name = name
            self._windows_uuid = uuid.UUID(windows_uuid)
            self._flags = flags
            self._mtu = mtu
            self._mac_address = mac_address
            self._ipv4_addresses = ipv4_addresses or ()
            self._ipv6_addresses = ipv6_addresses or ()
    else:
        __slots__ = ("_index", "_name", "_flags", "_mtu", "_mac_address", "_ipv4_addresses", "_ipv6_addresses")

        def __init__(
            self,
            index: int,
            name: str,
            flags: int,
            mtu: Optional[int] = None,
            mac_address: Optional[MacAddress] = None,
            ipv4_addresses: tuple[InterfaceIPAddress[ipaddress.IPv4Address], ...] = None,
            ipv6_addresses: tuple[InterfaceIPAddress[ipaddress.IPv6Address], ...] = None,
        ) -> None:
            self._index = index
            self._name = name
            self._flags = flags
            self._mtu = mtu
            self._mac_address = mac_address
            self._ipv4_addresses = ipv4_addresses or ()
            self._ipv6_addresses = ipv6_addresses or ()

    @property
    def index(self) -> int:
        return self._index

    @property
    def name(self) -> str:
        return self._name

    if IS_WINDOWS:
        @property
        def windows_uuid(self) -> uuid.UUID:
            return self._windows_uuid

    @property
    def mtu(self) -> Optional[int]:
        return self._mtu

    @property
    def mac_address(self) -> Optional[MacAddress]:
        return self._mac_address

    @property
    def ipv4_addresses(self) -> tuple[InterfaceIPAddress[ipaddress.IPv4Address], ...]:
        return self._ipv4_addresses

    @property
    def ipv6_addresses(self) -> tuple[InterfaceIPAddress[ipaddress.IPv6Address], ...]:
        return self._ipv6_addresses

    def is_flag_enabled(self, flag: InterfaceFlag) -> bool:
        return (self._flags & flag.value) == flag.value

    @override
    def __str__(self) -> str:
        string = f"{self._name} ({self._index}): flags={self._flags:x}<"
        if self._flags:
            if self._flags:
                i = 0
                for display, flag in _INTERFACE_FLAG_DISPLAYS.items():
                    if (self._flags & flag.value) == flag.value:
                        if i > 0:
                            string += ","
                        string += display
                        i += 1
        string += ">"
        if self._mtu:
            string += f" mtu {self._mtu}"
        string += "\n"

        if self._mac_address is not None:
            string += f"        ether {self._mac_address}\n"
        for address in self._ipv4_addresses:
            string += f"        inet  {address}\n"
        for address in self._ipv6_addresses:
            string += f"        inet6 {address}\n"

        return string

    @override
    def __repr__(self) -> str:
        return self.__str__()
