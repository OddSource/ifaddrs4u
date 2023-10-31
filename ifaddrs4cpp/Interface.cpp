/*
 * Copyright © 2010-2023 OddSource Code (license@oddsource.io)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Interface.h"

OddSource::Interfaces::Interface::
Interface(
    uint32_t index,
    ::std::string_view const & name,
#ifdef IS_WINDOWS
    ::std::string_view const & windows_uuid,
#endif /* IS_WINDOWS */
    uint32_t flags,
    ::std::optional<uint64_t const> const & mtu)
    : _index(index),
      _name(name),
#ifdef IS_WINDOWS
      _windows_uuid(windows_uuid),
#endif /* IS_WINDOWS */
      _flags(flags),
      _mtu(mtu),
      _mac_address(),
      _ipv4_addresses(),
      _ipv6_addresses()
{
}

namespace
{
    struct InterfaceFlagDisplayInfo
    {
        ::std::string display;
        OddSource::Interfaces::InterfaceFlag flag;
    };
}

::std::ostream &
OddSource::Interfaces::
operator<<(::std::ostream & os, OddSource::Interfaces::Interface const & interface)
{
    static ::std::vector<InterfaceFlagDisplayInfo const> const flag_displays {
        {"UP", IsUp},
        {"RUNNING", IsRunning},
        {"LOOPBACK", IsLoopback},
        {"POINTOPOINT", IsPointToPoint},
        {"BROADCAST", BroadcastAddressSet},
        {"MULTICAST", SupportsMulticast},
        {"DEBUG", DebugEnabled},
        {"PROMISC", PromiscuousModeEnabled},
        {"ALLMULTI", ReceiveAllMulticastPackets},
        {"NOARP", NoARP},
        {"SMART", NoTrailers},
#ifdef IFF_OACTIVE
        {"OACTIVE", TransmissionInProgress},
#endif /* IFF_OACTIVE */
#ifdef IFF_SIMPLEX
        {"SIMPLEX", Simplex},
#endif /* IFF_SIMPLEX */
#ifdef IFF_MASTER
        {"MASTER", Master},
#endif /* IFF_MASTER */
#ifdef IFF_SLAVE
        {"SLAVE", Slave},
#endif /* IFF_SLAVE */
    };

    os << interface._name << " (" << interface._index << "): flags="
       << ::std::hex << interface._flags << ::std::dec << "<";
    if (interface._flags)
    {
        uint8_t i(0);
        for (auto const & flag_display : flag_displays)
        {
            if ((interface._flags & flag_display.flag) == flag_display.flag)
            {
                if (i++ > 0)
                {
                    os << ",";
                }
                os << flag_display.display;
            }
        }
    }
    os << ">";
    if (interface._mtu)
    {
        os << " mtu " << ::std::to_string(*interface._mtu);
    }
    os << ::std::endl;
    if (interface._mac_address)
    {
        os << "        ether " << (*interface._mac_address) << ::std::endl;
    }
    for (auto const & address : interface._ipv4_addresses)
    {
        os << "        inet  " << address << ::std::endl;
    }
    for (auto const & address : interface._ipv6_addresses)
    {
        os << "        inet6 " << address << ::std::endl;
    }
    return os;
}

OddSource::Interfaces::Interface
OddSource::Interfaces::Interface::
get_sample_interface()
{
    Interface interface(
        3,
        "en0",
#ifdef IS_WINDOWS
        "24af9519-2a42-4f62-99fa-1ed3147ad90a",
#endif /* IS_WINDOWS */
        BroadcastAddressSet | IsUp | IsRunning,
        1725);
    interface._mac_address.emplace("ac:de:48:00:11:22");
    interface._ipv4_addresses.emplace_back(IPv4Address("192.168.0.42"), 0, 24, IPv4Address("192.168.0.254"));
    interface._ipv6_addresses.emplace_back(
            IPv6Address(static_cast<in6_addr const *>(IPv6Address("fe80::aede:48ff:fe00:1122")), v6Scope {6, "en5"}),
            Secured,
            64);
    interface._ipv6_addresses.emplace_back(
            IPv6Address("2001:470:2ccb:a61b:e:acf8:6736:d81f"),
            AutoConfigured | Secured,
            56);
    return interface;
}

OddSource::Interfaces::Interface const
OddSource::Interfaces::Interface::SAMPLE_INTERFACE = OddSource::Interfaces::Interface::get_sample_interface();
