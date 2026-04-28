/*
* Copyright © 2010-2026 OddSource Code (license@oddsource.io)
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

#pragma once

#ifndef IFADDRS4CPP_INLINE_SOURCE
#include "../Interface.hpp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace
{
    struct InterfaceFlagDisplayInfo
    {
        ::std::string display;
        OddSource::Interfaces::InterfaceFlag flag;
    };

    template< class K, class V >
    ::std::unordered_map< V, K const >
    flip(
        ::std::unordered_map< K, V const > const & source )
    {
        ::std::unordered_map< V, K const > destination;
        destination.reserve( source.size() );
        for ( auto const & [ key, value ] : source )
        {
            destination.emplace( value, key );
        }
        return destination;
    }
}

namespace OddSource::Interfaces
{
    ::std::unordered_map< ::std::string, InterfaceIPAddressFlag const > const
    InterfaceIPAddressFlag_Values
    {
        { "Anycast", InterfaceIPAddressFlag::Anycast },
        { "AutoConfigured", InterfaceIPAddressFlag::AutoConfigured },
        { "Deprecated", InterfaceIPAddressFlag::Deprecated },
        { "Detached", InterfaceIPAddressFlag::Detached },
        { "Duplicated", InterfaceIPAddressFlag::Duplicated },
        { "Dynamic", InterfaceIPAddressFlag::Dynamic },
        { "Optimistic", InterfaceIPAddressFlag::Optimistic },
        { "NoDad", InterfaceIPAddressFlag::NoDad },
        { "Secured", InterfaceIPAddressFlag::Secured },
        { "Temporary", InterfaceIPAddressFlag::Temporary },
        { "Tentative", InterfaceIPAddressFlag::Tentative },
    };

    ::std::unordered_map< InterfaceIPAddressFlag, ::std::string const > const
    InterfaceIPAddressFlag_Names = flip( InterfaceIPAddressFlag_Values );

    ::std::unordered_map< ::std::string, InterfaceFlag const > const
    InterfaceFlag_Values
    {
        { "BroadcastAddressSet", InterfaceFlag::BroadcastAddressSet},
        { "DebugEnabled", InterfaceFlag::DebugEnabled},
        { "IsLoopback", InterfaceFlag::IsLoopback},
        { "IsPointToPoint", InterfaceFlag::IsPointToPoint},
        { "IsRunning", InterfaceFlag::IsRunning},
        { "IsUp", InterfaceFlag::IsUp},
        { "NoARP", InterfaceFlag::NoARP},
        { "PromiscuousModeEnabled", InterfaceFlag::PromiscuousModeEnabled},
        { "ReceiveAllMulticastPackets", InterfaceFlag::ReceiveAllMulticastPackets},
        { "SupportsMulticast", InterfaceFlag::SupportsMulticast},
#ifdef IFF_MASTER
        { "Master", InterfaceFlag::Master },
#endif /* IFF_MASTER */
#ifdef IFF_SIMPLEX
        { "Simplex", InterfaceFlag::Simplex },
#endif /* IFF_SIMPLEX */
#ifdef IFF_SLAVE
        { "IFF_SLAVE", InterfaceFlag::Slave },
#endif /* IFF_SLAVE */
#ifdef IFF_NOTRAILERS
        { "Smart", InterfaceFlag::Smart },
#endif /* IFF_NOTRAILERS */
#ifdef IFF_OACTIVE
        { "TransmissionInProgress", InterfaceFlag::TransmissionInProgress },
#endif /* IFF_OACTIVE */
    };

    ::std::unordered_map< InterfaceFlag, ::std::string const > const
    InterfaceFlag_Names = flip( InterfaceFlag_Values );

    Broadcast_t const
    Broadcast{};

    PointToPoint_t const
    PointToPoint{};

    OddSource_Inline
    ::std::underlying_type_t< InterfaceIPAddressFlag >
    operator&(
        ::std::underlying_type_t< InterfaceIPAddressFlag > lhs,
        InterfaceIPAddressFlag const & rhs )
    {
        return lhs & static_cast< ::std::underlying_type_t< InterfaceIPAddressFlag > >( rhs );
    }

    OddSource_Inline
    ::std::underlying_type_t< InterfaceIPAddressFlag >
    operator|(
        InterfaceIPAddressFlag const & lhs,
        InterfaceIPAddressFlag const & rhs )
    {
        return static_cast< ::std::underlying_type_t< InterfaceIPAddressFlag > >( lhs ) |
               static_cast< ::std::underlying_type_t< InterfaceIPAddressFlag > >( rhs );
    }

    OddSource_Export
    ::std::underlying_type_t< InterfaceIPAddressFlag >
    operator|(
        ::std::underlying_type_t< InterfaceIPAddressFlag > lhs,
        InterfaceIPAddressFlag const & rhs )
    {
        return lhs | static_cast< ::std::underlying_type_t< InterfaceIPAddressFlag > >( rhs );
    }

    OddSource_Inline
    bool
    operator==(
        ::std::underlying_type_t< InterfaceIPAddressFlag > lhs,
        InterfaceIPAddressFlag const & rhs )
    {
        return lhs == static_cast< ::std::underlying_type_t< InterfaceIPAddressFlag > >( rhs );
    }

    OddSource_Inline
    ::std::underlying_type_t< InterfaceFlag >
    operator&(
        ::std::underlying_type_t< InterfaceFlag > lhs,
        InterfaceFlag const & rhs )
    {
        return lhs & static_cast< ::std::underlying_type_t< InterfaceFlag > >( rhs );
    }

    OddSource_Inline
    ::std::underlying_type_t< InterfaceFlag >
    operator|(
        InterfaceFlag const & lhs,
        InterfaceFlag const & rhs )
    {
        return static_cast< ::std::underlying_type_t< InterfaceFlag > >( lhs ) |
               static_cast< ::std::underlying_type_t< InterfaceFlag > >( rhs );
    }

    OddSource_Export
    ::std::underlying_type_t< InterfaceFlag >
    operator|(
        ::std::underlying_type_t< InterfaceFlag > lhs,
        InterfaceFlag const & rhs )
    {
        return lhs | static_cast< ::std::underlying_type_t< InterfaceFlag > >( rhs );
    }

    OddSource_Inline
    bool
    operator==(
        ::std::underlying_type_t< InterfaceFlag > lhs,
        InterfaceFlag const & rhs )
    {
        return lhs == static_cast< ::std::underlying_type_t< InterfaceFlag > >( rhs );
    }

    OddSource_Inline
    Interface::
    Interface(
        ::std::uint32_t index,
        ::std::string_view const & name,
        ::std::string_view const & friendlyName,
        ::std::string_view const & description,
        ::std::uint32_t flags,
        ::std::optional<::std::uint64_t const> const & mtu)
        : _index( index ),
          _name( name ),
          _friendlyName( friendlyName ),
          _description( description ),
          _flags( flags ),
          _mtu( mtu )
    {
    }

    OddSource_Inline
    Interface::
    Interface(
        Interface const & other )
        : _index( other._index ),
          _name( other._name ),
          _friendlyName( other._friendlyName ),
          _description( other._description ),
          _flags( other._flags ),
          _mtu( other._mtu ),
          _macAddress( other._macAddress ),
          _ipv4Addresses( other._ipv4Addresses ),
          _ipv6Addresses( other._ipv6Addresses )
    {
    }

    OddSource_Inline
    Interface::
    Interface(
        Interface && other ) noexcept
        : _index( other._index ),
          _name( ::std::move( other._name ) ),
          _friendlyName( ::std::move( other._friendlyName ) ),
          _description( ::std::move( other._description ) ),
          _flags( other._flags ),
          _mtu( ::std::move( other._mtu ) ),
          _macAddress( ::std::move( other._macAddress ) ),
          _ipv4Addresses( ::std::move( other._ipv4Addresses ) ),
          _ipv6Addresses( ::std::move( other._ipv6Addresses ) )
    {
        other._flags = 0;
    }

    OddSource_Inline
    Interface::
    ~Interface() noexcept
    {
    }

    OddSource_Inline
    ::std::uint32_t
    Interface::
    index() const
    {
        return this->_index;
    }

    OddSource_Inline
    ::std::string
    Interface::
    name() const
    {
        return this->_name;
    }

    OddSource_Inline
    ::std::string
    Interface::
    friendlyName() const
    {
        return this->_friendlyName;
    }

    OddSource_Inline
    ::std::string
    Interface::
    description() const
    {
        return this->_description;
    }

    OddSource_Inline
    bool
    Interface::
    is_up() const
    {
        return this->is_flag_enabled( InterfaceFlag::IsUp );
    }

    OddSource_Inline
    bool
    Interface::
    is_loopback() const
    {
        return this->is_flag_enabled( InterfaceFlag::IsLoopback );
    }

    OddSource_Inline
    bool
    Interface::
    is_flag_enabled(
        InterfaceFlag const & flag ) const
    {
        return ( this->_flags & flag ) == flag;
    }

    OddSource_Inline
    ::std::uint32_t
    Interface::
    flags() const
    {
        return this->_flags;
    }

    OddSource_Inline
    ::std::optional< ::std::uint64_t >
    Interface::
    mtu() const
    {
        return this->_mtu;
    }

    OddSource_Inline
    bool
    Interface::
    has_mac_address() const
    {
        return static_cast<bool>(this->_macAddress);
    }

    OddSource_Inline
    ::std::optional< MacAddress const > const &
    Interface::
    mac_address() const
    {
        return this->_macAddress;
    }

    OddSource_Inline
    ::std::vector< InterfaceIPv4Address > const &
    Interface::
    ipv4_addresses() const
    {
        return this->_ipv4Addresses;
    }

    OddSource_Inline
    ::std::vector< InterfaceIPv6Address > const &
    Interface::
    ipv6_addresses() const
    {
        return this->_ipv6Addresses;
    }

    Interface
    Interface::
    getSampleInterface()
    {
        Interface interface(
            3,
#ifdef ODDSOURCE_IS_WINDOWS
            "{24af9519-2a42-4f62-99fa-1ed3147ad90a}",
            "Wi-Fi",
            "Intel(R) Wi-Fi 6 AX201 160MHz",
#else /* ODDSOURCE_IS_WINDOWS */
            "en0",
            "en0",
            "en0",
#endif /* !ODDSOURCE_IS_WINDOWS */
            InterfaceFlag::BroadcastAddressSet | InterfaceFlag::IsUp | InterfaceFlag::IsRunning,
            1725 );
        interface._macAddress.emplace( "ac:de:48:00:11:22" );
        interface._ipv4Addresses.emplace_back(
            IPv4Address( "192.168.0.42" ),
            0,
            24,
            Broadcast,
            IPv4Address( "192.168.0.254" ) );
        interface._ipv6Addresses.emplace_back(
            IPv6Address(
                static_cast< in6_addr const * >( IPv6Address( "fe80::aede:48ff:fe00:1122" ) ),
                v6Scope {6, "en5"} ),
            0 | InterfaceIPAddressFlag::Secured,
            64 );
        interface._ipv6Addresses.emplace_back(
            IPv6Address( "2001:470:2ccb:a61b:e:acf8:6736:d81f" ),
            InterfaceIPAddressFlag::AutoConfigured | InterfaceIPAddressFlag::Secured,
            56 );
        return interface;
    }

    OddSource_Inline
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        Interface const & interface )
    {
        static ::std::vector< InterfaceFlagDisplayInfo > const FLAG_DISPLAYS {
            { "UP", InterfaceFlag::IsUp },
            { "RUNNING", InterfaceFlag::IsRunning },
            { "LOOPBACK", InterfaceFlag::IsLoopback },
            { "POINTOPOINT", InterfaceFlag::IsPointToPoint },
            { "BROADCAST", InterfaceFlag::BroadcastAddressSet },
            { "MULTICAST", InterfaceFlag::SupportsMulticast },
            { "DEBUG", InterfaceFlag::DebugEnabled },
            { "PROMISC", InterfaceFlag::PromiscuousModeEnabled },
            { "ALLMULTI", InterfaceFlag::ReceiveAllMulticastPackets },
            { "NOARP", InterfaceFlag::NoARP },
    #ifdef IFF_NOTRAILERS
            { "SMART", InterfaceFlag::Smart },
    #endif /* IFF_NOTRAILERS */
    #ifdef IFF_OACTIVE
            { "OACTIVE", InterfaceFlag::TransmissionInProgress },
    #endif /* IFF_OACTIVE */
    #ifdef IFF_SIMPLEX
            { "SIMPLEX", InterfaceFlag::Simplex },
    #endif /* IFF_SIMPLEX */
    #ifdef IFF_MASTER
            { "MASTER", InterfaceFlag::Master },
    #endif /* IFF_MASTER */
    #ifdef IFF_SLAVE
            { "SLAVE", InterfaceFlag::Slave },
    #endif /* IFF_SLAVE */
        };

        os << interface._index << ": " << interface._name;
        if ( interface._name != interface._friendlyName )
        {
            os << " (" << interface._friendlyName;
            if ( interface._friendlyName != interface._description )
            {
                os << " [" << interface._description << ']';
            }
            os << ')';
        }
        os << ", flags=" << ::std::hex << interface._flags << ::std::dec << "<";
        if ( interface._flags )
        {
            ::std::uint8_t i(0);
            for ( auto const & flag_display : FLAG_DISPLAYS )
            {
                if ( ( interface._flags & flag_display.flag ) == flag_display.flag )
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
        if ( interface._mtu )
        {
            os << " mtu " << ::std::to_string( *interface._mtu );
        }
        os << ::std::endl;
        if ( interface._macAddress )
        {
            os << "        ether " << *interface._macAddress << ::std::endl;
        }
        for ( auto const & address : interface._ipv4Addresses )
        {
            os << "        inet  " << address << ::std::endl;
        }
        for ( auto const & address : interface._ipv6Addresses )
        {
            os << "        inet6 " << address << ::std::endl;
        }
        return os;
    }
}

