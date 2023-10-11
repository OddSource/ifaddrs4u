#include <sstream>

namespace
{
    template<class IPAddressT>
    inline ::std::optional<uint8_t>
    sanitize_prefix_length(IPAddressT const & address, uint8_t prefix_length)
    {
        static_assert(::std::is_base_of_v<OddSource::Interfaces::IPAddress, IPAddressT>,
                      "the template parameter IPAddressT must derive from IPAddress.");
        if (prefix_length > address.maximum_prefix_length())
        {
            throw ::std::invalid_argument(
                (::std::ostringstream() << "Invalid prefix length "
                                        << ::std::to_string(prefix_length)
                                        << " for IPv"
                                        << ::std::to_string(address.version())).str());
        }
        return prefix_length == 0 ? ::std::nullopt : ::std::optional<uint8_t>(prefix_length);
    }
}

template<class IPAddressT>
OddSource::Interfaces::InterfaceIPAddress<IPAddressT>::
InterfaceIPAddress(IPAddressT const & address, uint16_t flags, uint8_t prefix_length)
    : _address(address),
      _prefix_length(sanitize_prefix_length(address, prefix_length)),
      _broadcast(::std::nullopt),
      _point_to_point(::std::nullopt),
      _flags(flags)
{
}

template<class IPAddressT>
OddSource::Interfaces::InterfaceIPAddress<IPAddressT>::
InterfaceIPAddress(
    IPAddressT const & address,
    uint16_t flags,
    uint8_t prefix_length,
    IPAddressT const & broadcast_or_destination,
    bool is_point_to_point)
    : _address(address),
      _prefix_length(sanitize_prefix_length(address, prefix_length)),
      _broadcast(is_point_to_point ? ::std::optional<IPAddressT>(::std::nullopt) : broadcast_or_destination),
      _point_to_point(is_point_to_point ? broadcast_or_destination : ::std::optional<IPAddressT>(::std::nullopt)),
      _flags(flags)
{
}

namespace
{
    struct InterfaceIPFlagDisplayInfo
    {
        ::std::string display;
        OddSource::Interfaces::InterfaceIPAddressFlag flag;
    };
}

template<class IPAddressT>
::std::ostream &
OddSource::Interfaces::
operator<<(::std::ostream & os, OddSource::Interfaces::InterfaceIPAddress<IPAddressT> const & address)
{
    static ::std::vector<InterfaceIPFlagDisplayInfo const> const flag_displays {
        {"autoconf", AutoConfigured},
        {"deprecated", Deprecated},
        {"secured", Secured},
        {"temporary", Temporary},
        {"anycast", Anycast},
        {"detached", Detached},
        {"duplicated", Duplicated},
        {"dynamic", Dynamic},
        {"optimistic", Optimistic},
        {"tentative", Tentative},
        {"nodad", NoDad},
    };

    using namespace OddSource::Interfaces;
    os << address._address;
    if (address._prefix_length)
    {
        os << "/" << ::std::to_string(*address._prefix_length);
    }
    if (address._broadcast)
    {
        os << " broadcast " << *address._broadcast;
    }
    else if(address._point_to_point)
    {
        os << " destination " << *address._point_to_point;
    }
    if (address._flags)
    {
        for (auto const & flag_display : flag_displays)
        {
            if ((address._flags & flag_display.flag) == flag_display.flag)
            {
                os << " " << flag_display.display;
            }
        }
    }
    return os;
}

template<class IPAddressT>
inline
OddSource::Interfaces::InterfaceIPAddress<IPAddressT>::
operator ::std::string() const
{
    return (::std::ostringstream() << *this).str();
}

template<class IPAddressT>
inline
OddSource::Interfaces::InterfaceIPAddress<IPAddressT>::
operator char const *() const
{
    return this->operator::std::string().c_str();
}

template<class IPAddressT>
inline IPAddressT const &
OddSource::Interfaces::InterfaceIPAddress<IPAddressT>::
address() const
{
    return this->_address;
}

template<class IPAddressT>
inline ::std::optional<uint8_t> const &
OddSource::Interfaces::InterfaceIPAddress<IPAddressT>::
prefix_length() const
{
    return this->_prefix_length;
}

template<class IPAddressT>
inline ::std::optional<IPAddressT const> const &
OddSource::Interfaces::InterfaceIPAddress<IPAddressT>::
broadcast_address() const
{
    return this->_broadcast;
}

template<class IPAddressT>
inline ::std::optional<IPAddressT const> const &
OddSource::Interfaces::InterfaceIPAddress<IPAddressT>::
point_to_point_destination() const
{
    return this->_point_to_point;
}

template<class IPAddressT>
inline bool
OddSource::Interfaces::InterfaceIPAddress<IPAddressT>::
is_flag_enabled(OddSource::Interfaces::InterfaceIPAddressFlag flag) const
{
    return (this->_flags & flag) == flag;
}

template<class IPAddressT>
inline bool
OddSource::Interfaces::InterfaceIPAddress<IPAddressT>::
operator==(InterfaceIPAddress <IPAddressT> const & other) const
{
    return this->_flags == other._flags &&
           this->_prefix_length == other._prefix_length &&
           this->_address == other._address &&
           this->_broadcast == other._broadcast &&
           this->_point_to_point == other._point_to_point;
}

template<class IPAddressT>
inline bool
OddSource::Interfaces::InterfaceIPAddress<IPAddressT>::
operator!=(InterfaceIPAddress <IPAddressT> const & other) const
{
    return !this->operator==(other);
}

inline uint32_t
OddSource::Interfaces::Interface::
index() const
{
    return this->_index;
}

inline ::std::string
OddSource::Interfaces::Interface::
name() const
{
    return this->_name;
}

#ifdef IS_WINDOWS
inline ::std::string
OddSource::Interfaces::Interface::
windows_uuid() const
{
    return this->_windows_uuid;
}
#endif /* IS_WINDOWS */

inline bool
OddSource::Interfaces::Interface::
is_flag_enabled(InterfaceFlag flag) const
{
    return (this->_flags & flag) == flag;
}

inline ::std::optional<uint64_t const> const &
OddSource::Interfaces::Interface::
mtu() const
{
    return this->_mtu;
}

inline bool
OddSource::Interfaces::Interface::
is_up() const
{
    return this->is_flag_enabled(IsUp);
}

inline bool
OddSource::Interfaces::Interface::
is_loopback() const
{
    return this->is_flag_enabled(IsLoopback);
}

inline bool
OddSource::Interfaces::Interface::
has_mac_address() const
{
    return static_cast<bool>(this->_mac_address);
}

inline ::std::optional<OddSource::Interfaces::MacAddress const> const &
OddSource::Interfaces::Interface::
mac_address() const
{
    return this->_mac_address;
}

inline ::std::vector<OddSource::Interfaces::InterfaceIPAddress<OddSource::Interfaces::IPv4Address> const> const &
OddSource::Interfaces::Interface::
ipv4_addresses() const
{
    return this->_ipv4_addresses;
}

inline ::std::vector<OddSource::Interfaces::InterfaceIPAddress<OddSource::Interfaces::IPv6Address> const> const &
OddSource::Interfaces::Interface::
ipv6_addresses() const
{
    return this->_ipv6_addresses;
}
