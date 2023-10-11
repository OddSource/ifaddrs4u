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

    os << interface._name << ": flags=" << ::std::hex << interface._flags << ::std::dec << "<";
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
