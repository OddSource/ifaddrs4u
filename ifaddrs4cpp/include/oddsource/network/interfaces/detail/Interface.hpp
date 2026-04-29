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

#pragma once

#include "../Interface.hpp"

#include <algorithm>
#include <sstream>
#include <string>

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace
{
    using namespace OddSource::Interfaces;

    template< class IPAddressT >
    ::std::optional< ::std::uint8_t >
    sanitizePrefixLength(
        IPAddressT const & address,
        ::std::uint8_t prefix_length )
    {
        using namespace ::std::string_literals;
        static_assert( ::std::is_base_of_v< IPAddress, IPAddressT >,
                       "the template parameter IPAddressT must derive from IPAddress." );
        if ( prefix_length > address.maximum_prefix_length() )
        {
            throw ::std::invalid_argument(
                "Invalid prefix length "s + ::std::to_string( prefix_length ) +
                " for IPv" + toString( address.version() ) );
        }
        return prefix_length == 0 ? ::std::nullopt : ::std::optional<::std::uint8_t>(prefix_length);
    }

    struct InterfaceIPFlagDisplayInfo
    {
        ::std::string display;
        InterfaceIPAddressFlag flag;
    };
}

namespace OddSource::Interfaces
{
    template< class IPAddressT >
    InterfaceIPAddress< IPAddressT >::
    InterfaceIPAddress(
        IPAddressT const & address,
        ::std::uint16_t flags,
        ::std::uint8_t prefixLength )
        : _address( address ),
          _prefixLength( sanitizePrefixLength( address, prefixLength ) ),
          _broadcast( ::std::nullopt ),
          _pointToPointDestination( ::std::nullopt ),
          _flags( flags )
    {
    }

    template< class IPAddressT >
    InterfaceIPAddress< IPAddressT >::
    InterfaceIPAddress(
        IPAddressT const & address,
        ::std::uint16_t flags,
        ::std::uint8_t prefixLength,
        Broadcast_t,
        IPAddressT const & broadcastAddress )
        : _address( address ),
          _prefixLength( sanitizePrefixLength( address, prefixLength ) ),
          _broadcast( broadcastAddress ),
          _flags( flags )
    {
    }

    template< class IPAddressT >
    InterfaceIPAddress< IPAddressT >::
    InterfaceIPAddress(
        IPAddressT const & address,
        ::std::uint16_t flags,
        ::std::uint8_t prefixLength,
        PointToPoint_t,
        IPAddressT const & pointToPointDestination )
        : _address( address ),
          _prefixLength( sanitizePrefixLength( address, prefixLength ) ),
          _pointToPointDestination( pointToPointDestination ),
          _flags( flags )
    {
    }

    template< class IPAddressT >
    InterfaceIPAddress< IPAddressT >::
    InterfaceIPAddress(
        InterfaceIPAddress const & other )
        : _address( other._address ),
          _prefixLength( other._prefixLength ),
          _broadcast( other._broadcast ),
          _pointToPointDestination( other._pointToPointDestination ),
          _flags( other._flags )
    {
    }

    template< class IPAddressT >
    InterfaceIPAddress< IPAddressT >::
    InterfaceIPAddress(
        InterfaceIPAddress && other ) noexcept
        : _address( ::std::move( other._address ) ),
          _prefixLength( ::std::move( other._prefixLength ) ), // NOLINT(*-move-const-arg)
          _broadcast( ::std::move( other._broadcast ) ),
          _pointToPointDestination( ::std::move( other._pointToPointDestination ) ),
          _flags( other._flags )
    {
    }

    template< class IPAddressT >
    InterfaceIPAddress< IPAddressT >::
    ~InterfaceIPAddress() noexcept // NOLINT(*-use-equals-default)
    {
    }

    template< class IPAddressT >
    InterfaceIPAddress< IPAddressT >::
    operator ::std::string() const
    {
        ::std::ostringstream oss;
        oss << *this;
        return oss.str();
    }

    template< class IPAddressT >
    InterfaceIPAddress< IPAddressT >::
    operator char const *() const
    {
        return this->operator::std::string().c_str();
    }

    template< class IPAddressT >
    IPAddressT const &
    InterfaceIPAddress< IPAddressT >::
    address() const
    {
        return this->_address;
    }

    template< class IPAddressT >
    ::std::optional< ::std::uint8_t >
    InterfaceIPAddress< IPAddressT >::
    prefix_length() const
    {
        return this->_prefixLength;
    }

    template< class IPAddressT >
    ::std::optional< IPAddressT const > const &
    InterfaceIPAddress< IPAddressT >::
    broadcast_address() const
    {
        return this->_broadcast;
    }

    template< class IPAddressT >
    ::std::optional< IPAddressT const > const &
    InterfaceIPAddress< IPAddressT >::
    point_to_point_destination() const
    {
        return this->_pointToPointDestination;
    }

    template< class IPAddressT >
    bool
    InterfaceIPAddress< IPAddressT >::
    is_flag_enabled(
        InterfaceIPAddressFlag flag ) const
    {
        return ( this->_flags & flag ) == flag;
    }

    template< class IPAddressT >
    ::std::uint16_t
    InterfaceIPAddress< IPAddressT >::
    flags() const
    {
        return this->_flags;
    }

    template< class IPAddressT >
    bool
    InterfaceIPAddress< IPAddressT >::
    operator==(
        InterfaceIPAddress< IPAddressT > const & other ) const
    {
        return this->_flags == other._flags &&
               this->_prefixLength == other._prefixLength &&
               this->_address == other._address &&
               this->_broadcast == other._broadcast &&
               this->_pointToPointDestination == other._pointToPointDestination;
    }

    template< class IPAddressT >
    bool
    InterfaceIPAddress< IPAddressT >::
    operator!=(
        InterfaceIPAddress< IPAddressT > const & other ) const
    {
        return !this->operator==(other);
    }

    template< class IPAddressT >
    ::std::string
    toString(
        InterfaceIPAddress< IPAddressT > const & address )
    {
        ::std::ostringstream oss;
        oss << address;
        return oss.str();
    }

    template
    OddSource_Export
    ::std::string
    toString< IPv4Address >(
        InterfaceIPAddress< IPv4Address > const & address );

    template
    OddSource_Export
    ::std::string
    toString< IPv6Address >(
        InterfaceIPAddress< IPv6Address > const & address );

    template< class IPAddressT >
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        InterfaceIPAddress< IPAddressT > const & address )
    {
        static ::std::vector< InterfaceIPFlagDisplayInfo > const FLAG_DISPLAYS {
            { "autoconf", InterfaceIPAddressFlag::AutoConfigured },
            { "deprecated", InterfaceIPAddressFlag::Deprecated },
            { "secured", InterfaceIPAddressFlag::Secured },
            { "temporary", InterfaceIPAddressFlag::Temporary },
            { "anycast", InterfaceIPAddressFlag::Anycast },
            { "detached", InterfaceIPAddressFlag::Detached },
            { "duplicated", InterfaceIPAddressFlag::Duplicated },
            { "dynamic", InterfaceIPAddressFlag::Dynamic },
            { "optimistic", InterfaceIPAddressFlag::Optimistic },
            { "tentative", InterfaceIPAddressFlag::Tentative },
            { "nodad", InterfaceIPAddressFlag::NoDad },
        };

        auto const & ipAddress( address.address() );
        os << ipAddress;

        auto const prefixLength( address.prefix_length() );
        if ( prefixLength )
        {
            os << "/" << ::std::to_string( *prefixLength );
        }

        auto const & broadcast( address.broadcast_address() );
        auto const & pointToPoint( address.point_to_point_destination() );
        if ( broadcast )
        {
            os << " broadcast " << *broadcast;
        }
        else if( pointToPoint )
        {
            os << " destination " << *pointToPoint;
        }

        auto const flags( address.flags() );
        if ( flags )
        {
            for ( auto const & flag_display : FLAG_DISPLAYS )
            {
                if ( ( flags & flag_display.flag ) == flag_display.flag )
                {
                    os << " " << flag_display.display;
                }
            }
        }
        if constexpr ( ::std::is_same_v< IPAddressT, IPv6Address > )
        {
            auto & v6( static_cast< IPv6Address const & >( ipAddress ) );
            if ( v6.has_scope_id() )
            {
                os << " scopeid ";
                if ( v6.scope_id() )
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

    template
    OddSource_Export
    ::std::ostream &
    operator<<< IPv4Address >(
        ::std::ostream &,
        InterfaceIPAddress< IPv4Address > const & );

    template
    OddSource_Export
    ::std::ostream &
    operator<<< IPv6Address >(
        ::std::ostream &,
        InterfaceIPAddress< IPv6Address > const & );
}
