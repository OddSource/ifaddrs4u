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

#include <algorithm>
#include <sstream>
#include <string>

namespace
{
    using namespace OddSource::Interfaces;

    template<class IPAddressT>
    inline ::std::optional<uint8_t>
    sanitize_prefix_length(IPAddressT const & address, uint8_t prefix_length)
    {
#include "s.h" // limit the scope of the `using` to this function body
        static_assert(::std::is_base_of_v<IPAddress, IPAddressT>,
                      "the template parameter IPAddressT must derive from IPAddress.");
        if (prefix_length > address.maximum_prefix_length())
        {
            throw ::std::invalid_argument(
                "Invalid prefix length "s + ::std::to_string(prefix_length) +
                " for IPv" + ::std::to_string(address.version()));
        }
        return prefix_length == 0 ? ::std::nullopt : ::std::optional<uint8_t>(prefix_length);
    }

    struct Populator
    {
        Populator()
        {
            ::std::transform(::std::cbegin(InterfaceIPAddressFlag_Values),
                             ::std::cend(InterfaceIPAddressFlag_Values),
                             ::std::inserter(InterfaceIPAddressFlag_Names,
                                             ::std::begin(InterfaceIPAddressFlag_Names)),
                                             [](const auto& e) { return ::std::make_pair(e.second, e.first); });

            ::std::transform(::std::cbegin(InterfaceFlag_Values),
                             ::std::cend(InterfaceFlag_Values),
                             ::std::inserter(InterfaceFlag_Names,
                                             ::std::begin(InterfaceFlag_Names)),
                                             [](const auto& e) { return ::std::make_pair(e.second, e.first); });
        }
    };

    [[maybe_unused]]
    Populator const populator;
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
    static ::std::vector<InterfaceIPFlagDisplayInfo> const flag_displays {
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
    if constexpr (::std::is_same_v<IPAddressT, IPv6Address>)
    {
        auto v6 = static_cast<IPv6Address>(address._address);
        if (v6.has_scope_id())
        {
            os << " scopeid ";
            if (v6.scope_id())
            {
                os << *v6.scope_id();
            }
            else
            {
                os << *v6.scope_name();
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
    ::std::ostringstream oss;
    oss << *this;
    return oss.str();
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
inline uint16_t
OddSource::Interfaces::InterfaceIPAddress<IPAddressT>::
flags() const
{
    return this->_flags;
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

inline uint32_t
OddSource::Interfaces::Interface::
flags() const
{
    return this->_flags;
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

inline ::std::vector<OddSource::Interfaces::InterfaceIPAddress<OddSource::Interfaces::IPv4Address>> const &
OddSource::Interfaces::Interface::
ipv4_addresses() const
{
    return this->_ipv4_addresses;
}

inline ::std::vector<OddSource::Interfaces::InterfaceIPAddress<OddSource::Interfaces::IPv6Address>> const &
OddSource::Interfaces::Interface::
ipv6_addresses() const
{
    return this->_ipv6_addresses;
}
