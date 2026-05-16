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
#include "../IpAddress.hpp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

#include "../detail/bitwise_enum.ipp"
#include "../detail/flip.hpp"
// ReSharper disable once CppUnusedIncludeDirective
#include "../detail/winsock_includes.h"

#ifndef ODDSOURCE_IS_WINDOWS

#include <arpa/inet.h>
#include <cerrno>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>

#endif /* !ODDSOURCE_IS_WINDOWS */

#include <array>
#include <cstring>
#include <string>
#include <string_view>
#include <sstream>
#include <utility>

#define BYTES reinterpret_cast< ::std::uint8_t const * >( this->_data.get() )
#define WORDS reinterpret_cast< ::std::uint16_t const * >( this->_data.get() )
#define DOUBLEWORDS reinterpret_cast< ::std::uint32_t const * >( this->_data.get() )

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace
{
    using namespace OddSource::Interfaces;

    template< typename Addr >
    using Enable_If_Addr = ::std::enable_if_t< ::std::is_same_v< Addr, in_addr > ||
                                               ::std::is_same_v< Addr, in6_addr > >;

    template< typename Addr, typename = Enable_If_Addr< Addr > >
    ::std::unique_ptr< Addr >
    fromRepr(
        ::std::string_view const & repr )
    {
        using namespace ::std::string_literals;
        if ( repr.empty() )
        {
            throw InvalidIPAddress( "Invalid empty IP address string." );
        }

        ::std::string const reprStr( repr );
        auto data( ::std::make_unique< Addr >() );
        int success;
        if constexpr ( ::std::is_same_v< Addr, in6_addr > )
        {
            // inet_pton can also handle IPv4 addresses, but only in dotted-decimal format
            // (1.2.3.4), not in octal, hexadecimal, or any other valid IPv4 format.
            success = inet_pton( AF_INET6, reprStr.c_str(), data.get() );
        }
        else
        {
            int numDots{ 0 };
            for (char const c : repr)
            {
                if (c == '.')
                {
                    numDots++;
                }
            }
            if (numDots != 3)
            {
                // some implementations of inet_aton tolerate incomplete addresses, but we do not
                throw InvalidIPAddress(
                        "Malformed IPv4 address string '"s + reprStr + "' with "s +
                        ::std::to_string( numDots + 1 ) + " parts instead of 4"s );
            }
            // inet_aton/RtlIpv4StringToAddress, however, can handle IPv4 addresses in all valid formats.
#ifdef ODDSOURCE_IS_WINDOWS
            char const * end = nullptr;
            success = RtlIpv4StringToAddress( reprStr.c_str(), false, &end, data.get() );
            if ( success == STATUS_INVALID_PARAMETER )
            {
                throw InvalidIPAddress(
                    "An invalid parameter was passed to RtlIpv4StringToAddress while converting '"s +
                    reprStr + "'"s );
            }
            else if ( success != 0 )
            {
                char * s = nullptr;
                ::FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    nullptr, success,
                    MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                    (LPTSTR)&s, 0, nullptr );
                ::std::string const err( s == nullptr ? "" : s );
                LocalFree( s );
                throw InvalidIPAddress(
                    "Malformed IP address string '"s + reprStr + "' or unknown RtlIpv4StringToAddress error ("s +
                    ::std::to_string( success ) + "): "s + err );
            }
            success = 1;
#else /* ODDSOURCE_IS_WINDOWS */
            success = inet_aton( reprStr.c_str(), data.get() );
#endif /* !ODDSOURCE_IS_WINDOWS */
        }
        if (success != 1)
        {
            throw InvalidIPAddress(
                "Malformed IP address string '"s + reprStr + "' or unknown inet_*ton error."s );
        }

        return data;
    }

    template< typename Addr, typename = Enable_If_Addr< Addr > >
    ::std::string
    toRepr(
        Addr const * data ) {
        using namespace ::std::string_literals;
        AddressFamily family;
        if constexpr ( ::std::is_same_v< Addr, in6_addr > )
        {
            family = AF_INET6;
        }
        else
        {
            family = AF_INET;
        }

        static constexpr size_t HOST_LENGTH{ 100 };
        char hostChars[ HOST_LENGTH ];
        auto ptr( ::inet_ntop( family, data, hostChars, HOST_LENGTH ) );
        if ( ptr == nullptr )
        {
#ifdef ODDSOURCE_IS_WINDOWS
            auto errorCode( ::WSAGetLastError() );
            char * s = nullptr;
            ::FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, errorCode,
                MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                (LPTSTR)&s, 0, nullptr );
            ::std::string const err( s == nullptr ? "" : s );
            LocalFree( s );
#else /* ODDSOURCE_IS_WINDOWS */
            auto const errorCode{ errno };
            char const * err(
                errorCode == EAFNOSUPPORT ? "Address family not supported" :
                ( errorCode == ENOSPC ? "Converted address would exceed string size" : ::gai_strerror( errorCode ) ) );
#endif /* !ODDSOURCE_IS_WINDOWS */
            throw InvalidIPAddress(
                "Malformed in_addr data or inet_ntop system error: "s +
                ::std::to_string( errorCode ) + " ('"s + err + "')"s );
        }
        return hostChars;
    }

    template< typename Addr, typename = Enable_If_Addr< Addr > >
    ::std::string
    toRepr(
        ::std::unique_ptr< Addr const > const & data )
    {
        return toRepr( data.get() );
    }

    template< typename Addr, typename = Enable_If_Addr< Addr > >
    ::std::unique_ptr< Addr >
    copy_in_addr(
        Addr const * data )
    {
        auto newData( ::std::make_unique< Addr >() );
        ::std::memcpy( newData.get(), data, sizeof( Addr ) );
        return newData;
    }

    template< typename Addr, typename = Enable_If_Addr< Addr > >
    ::std::unique_ptr< Addr >
    copy_in_addr(
        ::std::unique_ptr< Addr const > const & data )
    {
        return copy_in_addr( data.get() );
    }

    ::std::unique_ptr< in_addr >
    to_in_addr(
        ::std::uint32_t const data )
    {
        auto addr( ::std::make_unique< in_addr >() );
        addr->s_addr = htonl( data );
        return addr;
    }

    ::std::unique_ptr< in6_addr >
    to_in6_addr(
        IPv6Address::Bytes const & data )
    {
        auto addr( ::std::make_unique< in6_addr >() );
        ::std::memcpy( &addr->s6_addr, data.data(), sizeof( addr->s6_addr ) );
        return addr;
    }

    v6Scope &&
    fillOutScope(
        v6Scope && scope )
    {
        if ( scope.scopeId && !scope.scopeName )
        {
            char buffer[ IF_NAMESIZE ];
            if ( ::if_indextoname( *scope.scopeId, buffer ) != nullptr )
            {
                scope.scopeName = ::std::string( buffer );
            }
        }
        else if( scope.scopeName && !scope.scopeId )
        {
            if ( ::std::uint32_t const scopeId{ ::if_nametoindex( scope.scopeName->c_str() ) }; scopeId > 0 )
            {
                scope.scopeId = scopeId;
            }
        }
        return std::move( scope );
    }

    v6Scope
    scopeFrom(
        ::std::uint32_t const scopeId )
    {
        if ( scopeId == 0 )
        {
            throw ::std::invalid_argument( "IPv6 address scope ID must be greater than 0." );
        }
        return fillOutScope( { scopeId } );
    }

    v6Scope
    scopeFrom(
        ::std::string_view const & scopeName )
    {
        if ( scopeName.empty() )
        {
            throw ::std::invalid_argument( "IPv6 address scope name must not be an empty string." );
        }
        return fillOutScope( { ::std::nullopt, ::std::string( scopeName ) } );
    }

    ::std::string_view
    stripScope(
        ::std::string_view const & repr )
    {
        if ( auto const i{ repr.find( '%' ) }; i != ::std::string_view::npos )
        {
            return repr.substr(0, i);
        }
        return repr;
    }

    ::std::optional< v6Scope >
    extractScope(
        ::std::string_view const & repr )
    {
        if ( auto const i{ repr.find( '%' ) }; i != ::std::string_view::npos )
        {
            ::std::string const scope( repr.substr( i + 1 ) );
            if ( !scope.empty() )
            {
                if ( scope.find_first_not_of( "0123456789" ) == std::string::npos )
                {
                    try
                    {
                        return scopeFrom( ::std::stoul( scope ) );
                    }
                    catch ( ::std::invalid_argument const & )
                    {
                    }
                }
                return scopeFrom( scope );
            }
        }
        return ::std::nullopt;
    }

    ::std::string
    addScope(
        ::std::string const & repr,
        ::std::optional< v6Scope > const & scope )
    {
        if ( !scope )
        {
            return repr;
        }
        return repr + "%" + ( scope->scopeName ? *scope->scopeName : ::std::to_string( *scope->scopeId ) );
    }
}

namespace OddSource::Interfaces
{
    OddSource_Inline
    InvalidIPAddress::
    InvalidIPAddress(
        ::std::string_view const & what )
        : ::std::invalid_argument( ::std::string( what ) )
    {
    }

    OddSource_Inline
    InvalidIPAddress::
    InvalidIPAddress( // NOLINT(*-use-equals-default)
        InvalidIPAddress const & other )
        : ::std::invalid_argument( other )
    {
    }

    OddSource_Inline
    InvalidIPAddress::
    ~InvalidIPAddress() noexcept  // NOLINT(*-use-equals-default)
    {
    }

    OddSource_Inline
    ::std::string
    toString(
        IPAddressVersion const & version )
    {
        return ::std::to_string( static_cast< ::std::uint16_t >( version ) );
    }

    OddSource_Inline
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        IPAddressVersion const & version )
    {
        return os << toString( version );
    }

    ::std::unordered_map< ::std::string, MulticastScope const > const
    MulticastScope_Values
    {
        { "Reserved", MulticastScope::Reserved },
        { "InterfaceLocal", MulticastScope::InterfaceLocal },
        { "LinkLocal", MulticastScope::LinkLocal },
        { "RealmLocal", MulticastScope::RealmLocal },
        { "AdminLocal", MulticastScope::AdminLocal },
        { "SiteLocal", MulticastScope::SiteLocal },
        { "OrganizationLocal", MulticastScope::OrganizationLocal },
        { "Global", MulticastScope::Global },
        { "Unassigned", MulticastScope::Unassigned },
    };

    ::std::unordered_map< MulticastScope, ::std::string const > const
    MulticastScope_Names = detail::flip( MulticastScope_Values );

    OddSource_Inline
    ::std::string
    toString(
        MulticastScope const & scope )
    {
        return MulticastScope_Names.at( scope );
    }

    OddSource_Inline
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        MulticastScope const & scope )
    {
        return os << toString( scope );
    }

    ODDSOURCE_BITWISE_ENUM_OPERATORS_IMPL( MulticastV6Flag );

    OddSource_Inline
    IPAddress::
    IPAddress(
        ::std::string_view const & repr )
        : _representation(repr)
    {
    }

    OddSource_Inline
    IPAddress::
    IPAddress( // NOLINT(*-use-equals-default)
        IPAddress const & other )
        : _representation( other._representation ),
          _isUnspecified( other._isUnspecified ),
          _isLoopback( other._isLoopback ),
          _isLinkLocal( other._isLinkLocal ),
          _isPrivate( other._isPrivate ),
          _isMulticast( other._isMulticast ),
          _isReserved( other._isReserved ),
          _multicastScope( other._multicastScope )
    {
    }

    OddSource_Inline
    IPAddress::
    IPAddress(
        IPAddress && other ) noexcept
        : _representation( std::move( other._representation ) ),
          _isUnspecified( other._isUnspecified ),
          _isLoopback( other._isLoopback ),
          _isLinkLocal( other._isLinkLocal ),
          _isPrivate( other._isPrivate ),
          _isMulticast( other._isMulticast ),
          _isReserved( other._isReserved ),
          _multicastScope( ::std::move( other._multicastScope ) ) // NOLINT(*-move-const-arg)
    {
        other._isUnspecified = false;
        other._isLoopback = false;
        other._isLinkLocal = false;
        other._isPrivate = false;
        other._isMulticast = false;
        other._isReserved = false;
    }

    OddSource_Inline
    IPAddress::
    ~IPAddress() noexcept // NOLINT(*-use-equals-default)
    {
    }

    OddSource_Inline
    ::std::unique_ptr< IPAddress >
    IPAddress::
    create(
        ::std::string_view repr )
    {
        try
        {
            return ::std::make_unique< IPv4Address >( repr );
        }
        catch ( InvalidIPAddress const & e1 )
        {
            try
            {
                return ::std::make_unique< IPv6Address >( repr );
            }
            catch ( InvalidIPAddress const & e2 )
            {
                ::std::ostringstream oss;
                oss << "The address string \"" << repr
                    << "\" could not be converted to either an IPv4 or an IPv6 address. The reason it could not be "
                       "converted to an IPv4 address: [" << e1.what()
                    << "]. The reason it could not be converted to an IPv6 address: [" << e2.what() << "].";
                throw InvalidIPAddress( oss.str() );
            }
        }
    }

    OddSource_Inline
    IPAddress::
    operator ::std::string() const
    {
        return this->_representation;
    }

    OddSource_Inline
    IPAddress::
    operator char const *() const
    {
        return this->_representation.c_str();
    }

    OddSource_Inline
    bool
    IPAddress::
    isUnspecified() const
    {
        return this->_isUnspecified;
    }

    OddSource_Inline
    bool
    IPAddress::
    isLoopback() const
    {
        return this->_isLoopback;
    }

    OddSource_Inline
    bool
    IPAddress::
    isLinkLocal() const
    {
        return this->_isLinkLocal;
    }

    OddSource_Inline
    bool
    IPAddress::
    isPrivate() const
    {
        return this->_isPrivate;
    }

    OddSource_Inline
    bool
    IPAddress::
    isMulticast() const
    {
        return this->_isMulticast;
    }

    OddSource_Inline
    bool
    IPAddress::
    isReserved() const
    {
        return this->_isReserved;
    }

    OddSource_Inline
    ::std::optional< MulticastScope > const &
    IPAddress::
    multicastScope() const
    {
        return this->_multicastScope;
    }

    OddSource_Inline
    IPv4Address::
    IPv4Address(
        ::std::string_view const & repr )
        : IPv4Address( fromRepr< in_addr >( repr ) )
    {
    }

    OddSource_Inline
    IPv4Address::
    IPv4Address(
        in_addr const * data )
        : IPv4Address( copy_in_addr( data ) )
    {
    }

    OddSource_Inline
    IPv4Address::
    IPv4Address(
        ::std::uint32_t const data )
        : IPv4Address( to_in_addr( data ) )
    {
    }

    OddSource_Inline
    IPv4Address::
    IPv4Address(
        ::std::unique_ptr< in_addr const > && data )
        : IPAddress( toRepr( data ) ),
          _data( ::std::move( data ) )
    {
        auto const bytes = BYTES;

        if ( *reinterpret_cast< ::std::uint32_t const * >( this->_data.get() ) == 0)
        {
            this->_isUnspecified = true;
            this->_isReserved = true;
        }
        else if ( bytes[ 0 ] == 127 ) // 127.0.0.0/8
        {
            this->_isLoopback = true;
            this->_isReserved = true;
        }
        else if ( bytes[ 0 ] == 169 && bytes[ 1 ] == 254 ) // 169.254.0.0/16
        {
            this->_isLinkLocal = true;
            this->_isReserved = true;
        }
        else if (
            bytes[ 0 ] == 10 || // 10.0.0.0/8
            ( bytes[ 0 ] == 100 && bytes[ 1 ] >= 64 && bytes[ 1 ] <= 127 ) || // 100.64.0.0/10
            ( bytes[ 0 ] == 172 && bytes[ 1 ] >= 16 && bytes[ 1 ] <= 31 ) || // 172.16.0.0/12
            ( bytes[ 0 ] == 192 && bytes[ 1 ] == 0 && bytes[ 2 ] == 0 ) || // 192.0.0.0/24
            ( bytes[ 0 ] == 192 && bytes[ 1 ] == 168 ) || // 192.168.0.0/16
            ( bytes[ 0 ] == 198 && bytes[ 1 ] >= 18 && bytes[ 1 ] <= 19 ) // 198.18.0.0/15
        )
        {
            this->_isPrivate = true;
            this->_isReserved = true;
        }
        else if ( bytes[ 0 ] >= 224 && bytes[ 0 ] <= 239 ) // 224.0.0.0/4
        {
            this->_isMulticast = true;
            this->_isReserved = true;
        }
        // various other reserved ranges, see https://en.wikipedia.org/wiki/Reserved_IP_addresses
        else if (
            bytes[ 0 ] == 0 || // 0.0.0.0/8
            (bytes[ 0 ] == 192 && bytes[ 1 ] == 0 && bytes[ 2 ] == 2) || // 192.0.2.0/24
            (bytes[ 0 ] == 192 && bytes[ 1 ] == 88 && bytes[ 2 ] == 99) || // 192.88.99.0/24
            (bytes[ 0 ] == 198 && bytes[ 1 ] == 51 && bytes[ 2 ] == 100) || // 198.51.100.0/24
            (bytes[ 0 ] == 203 && bytes[ 1 ] == 0 && bytes[ 2 ] == 113) || // 203.0.113.0/24
            (bytes[ 0 ] == 233 && bytes[ 1 ] == 252 && bytes[ 2 ] == 0) || // 233.252.0.0/24
            bytes[ 0 ] >= 240 // 240.0.0.0/4
        )
        {
            this->_isReserved = true;
        }

        if ( this->isMulticast() )
        {
            if ( bytes[ 0 ] == 224 && bytes[ 1 ] == 0 && bytes[ 2 ] == 0 ) // 224.0.0.0/24
            {
                this->_multicastScope = MulticastScope::LinkLocal;
            }
            else if ( bytes[ 0 ] == 239 && bytes[ 1 ] == 255 ) // 239.255.0.0/16
            {
                this->_multicastScope = MulticastScope::RealmLocal;
            }
            else if ( bytes[ 0 ] == 239 && bytes[ 1 ] >= 192 && bytes[ 1 ] <= 195 ) // 239.192.0.0/14
            {
                this->_multicastScope = MulticastScope::OrganizationLocal;
            }
            else if ( bytes[ 0 ] != 239 ) // // 224.0.1.0-238.255.255.255
            {
                this->_multicastScope = MulticastScope::Global;
            }
            else
            {
                this->_multicastScope = MulticastScope::Unassigned;
            }
        }
    }

    OddSource_Inline
    IPv4Address::
    IPv4Address(
       IPv4Address const & other )
        : IPAddress( other ),
          _data( copy_in_addr( other._data ) )
    {
    }

    OddSource_Inline
    IPv4Address::
    IPv4Address(
       IPv4Address && other ) noexcept
        : IPAddress( std::move( other ) ),
          _data( std::move( other._data ) )
    {
    }

    OddSource_Inline
    IPv4Address::
    ~IPv4Address() noexcept // NOLINT(*-use-equals-default)
    {
    }

    OddSource_Inline
    IPv4Address::
    operator in_addr const *() const
    {
        return this->_data.get();
    }

    OddSource_Inline
    IPv4Address::
    operator ::std::uint32_t() const
    {
        return ntohl( this->_data->s_addr );
    }

    OddSource_Inline
    bool
    IPv4Address::
    operator==(
        IPv4Address const & other ) const
    {
        return static_cast< ::std::uint32_t >( *this ) == static_cast< ::std::uint32_t >( other );
    }

    OddSource_Inline
    bool
    IPv4Address::
    operator!=(
        IPv4Address const & other ) const
    {
        return !this->operator==( other );
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        ::std::string_view const & repr )
        : IPv6Address( repr, stripScope( repr ) )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        in6_addr const * data )
        : IPv6Address( copy_in_addr( data ), std::nullopt )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        in6_addr const * data,
        ::std::uint32_t const scopeId )
        : IPv6Address(
            copy_in_addr( data ),
            scopeId > 0 ? ::std::optional( scopeFrom( scopeId ) ) : ::std::nullopt )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        in6_addr const * data,
        ::std::string_view const & scopeName )
        : IPv6Address( copy_in_addr( data ), scopeFrom( scopeName ) )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        in6_addr const * data,
        v6Scope const & scope )
        : IPv6Address( copy_in_addr( data ), scope )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        Bytes const & data )
        : IPv6Address( to_in6_addr( data ), ::std::nullopt )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        Bytes const & data,
        ::std::uint32_t const scopeId )
        : IPv6Address(
            to_in6_addr( data ),
            scopeId > 0 ? ::std::optional( scopeFrom( scopeId ) ) : ::std::nullopt )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        Bytes const & data,
        ::std::string_view const & scopeName )
        : IPv6Address( to_in6_addr( data ), scopeFrom( scopeName ) )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        Bytes const & data,
        v6Scope const & scope )
        : IPv6Address( to_in6_addr( data ), scope )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        ::std::string_view const & reprWithScope,
        ::std::string_view const & reprWithoutScope )
        : IPv6Address(
            std::string( reprWithoutScope ),
            fromRepr< in6_addr >( reprWithoutScope ),
            extractScope( reprWithScope ) )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        ::std::unique_ptr< in6_addr const > && data,
        ::std::optional< v6Scope > && scope )
        : IPv6Address( toRepr( data.get() ), std::move( data ), std::move( scope ) )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        ::std::string && reprWithoutScope,
        ::std::unique_ptr< in6_addr const > && data,
        ::std::optional< v6Scope > && scope )
        : IPAddress( addScope( reprWithoutScope, scope ) ),
          _data( ::std::move( data ) ),
          _scope( std::move( scope ) ),
          _withoutScope( reprWithoutScope )
    {
        auto const bytes = BYTES;
        auto const words = WORDS;
        auto const doublewords = DOUBLEWORDS;

        if ( IN6_IS_ADDR_UNSPECIFIED( this->_data.get() ) )
        {
            this->_isUnspecified = true;
            this->_isReserved = true;
        }
        else if( IN6_IS_ADDR_LOOPBACK( this->_data.get() ) )
        {
            this->_isLoopback = true;
            this->_isReserved = true;
        }
        else if( IN6_IS_ADDR_LINKLOCAL( this->_data.get() ) && // some impls erroneously check *only* fe80
                 words[ 1 ] == 0 && words[ 2 ] == 0 && words[ 3 ] == 0 )
        {
            this->_isLinkLocal = true;
            this->_isReserved = true;
        }
        // IN6_IS_ADDR_UNIQUE_LOCAL is available only on BSD-based systems like macOS
        else if( bytes[ 0 ] == 0xfc || bytes[ 0 ] == 0xfd )
        {
            this->_isUniqueLocal = true;
            this->_isPrivate = true;
            this->_isReserved = true;
        }
        else if( IN6_IS_ADDR_SITELOCAL( this->_data.get() ) )
        {
            this->_isSiteLocal = true;
            this->_isPrivate = true;
            this->_isReserved = true;
        }
        else if( IN6_IS_ADDR_MULTICAST( this->_data.get() ) )
        {
            this->_isMulticast = true;
            this->_isReserved = true;
        }
        else if( IN6_IS_ADDR_V4MAPPED( this->_data.get() ) )
        {
            this->_isV4Mapped = true;
            this->_isReserved = true;
        }
        else if( IN6_IS_ADDR_V4COMPAT( this->_data.get() ) )
        {
            this->_isV4Compatible = true;
            this->_isReserved = true;
        }
        // IN6_IS_ADDR_V4TRANSLATED is available only on Windows SDK / Winsock 2
        else if( doublewords[ 0 ] == 0 && doublewords[ 1 ] == 0 && words[ 4 ] == 0xffff && words[ 5 ] == 0 )
        {
            this->_isV4Translated = true;
            this->_isReserved = true;
        }
        // IN6_IS_ADDR_6TO4 is available only on BSD-based systems like macOS
        else if( ntohs( words[ 0 ] ) == 0x2002 )
        {
            this->_is6to4 = true;
            this->_isReserved = true;
        }
        // various other reserved ranges, see https://en.wikipedia.org/wiki/Reserved_IP_addresses
        else if(
            ( ntohs( words[ 0 ] ) == 0x64 && ntohs( words[1] ) == 0xff9b && doublewords[ 1 ] == 0 && doublewords[ 2 ] == 0 ) || // 64:ff9b::/96
            ( ntohs( words[ 0 ] ) == 0x64 && ntohs( words[1] ) == 0xff9b && doublewords[1] == 1 ) || // 64:ff9b:1::/48
            ( ntohs( words[ 0 ] ) == 0x100 && words[ 1 ] == 0 && doublewords[ 1 ] == 0 ) || // 100::/64
            ( ntohs( words[ 0 ] ) == 0x2001 && words[ 1 ] == 0 ) || // 2001:0000::/32
            ( ntohs( words[ 0 ] ) == 0x2001 && ntohs( words[1] ) >= 0x20 && ntohs( words[ 1 ] ) <= 0x2f ) || // 2001:20::/28
            ( ntohs( words[ 0 ] ) == 0x2001 && ntohs( words[1] ) == 0xdb8 ) // 2001:db8::/32
        )
        {
            this->_isReserved = true;
        }

        if ( this->isMulticast() )
        {
            this->_multicastFlags = ( bytes[ 1 ] & 0b11110000 ) >> 4;
            switch ( bytes[ 1 ] & 0b1111 )
            {
                case 0x0:
                case 0xf: this->_multicastScope = MulticastScope::Reserved; break;
                case 0x1: this->_multicastScope = MulticastScope::InterfaceLocal; break;
                case 0x2: this->_multicastScope = MulticastScope::LinkLocal; break;
                case 0x3: this->_multicastScope = MulticastScope::RealmLocal; break;
                case 0x4: this->_multicastScope = MulticastScope::AdminLocal; break;
                case 0x5: this->_multicastScope = MulticastScope::SiteLocal; break;
                case 0x8: this->_multicastScope = MulticastScope::OrganizationLocal; break;
                case 0xe: this->_multicastScope = MulticastScope::Global; break;
                default: this->_multicastScope = MulticastScope::Unassigned; break;
            }
        }
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        IPv6Address const & other )
        : IPAddress( other ),
          _data( copy_in_addr( other._data ) ),
          _scope( other._scope ),
          _withoutScope( other._withoutScope ),
          _isUniqueLocal( other._isUniqueLocal ),
          _isSiteLocal( other._isSiteLocal ),
          _isV4Mapped( other._isV4Mapped ),
          _isV4Compatible( other._isV4Compatible ),
          _isV4Translated( other._isV4Translated ),
          _is6to4( other._is6to4 ),
          _multicastFlags( other._multicastFlags )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        IPv6Address && other ) noexcept
        : IPAddress( std::move( other ) ),
          _data( std::move( other._data ) ),
          _scope( std::move( other._scope ) ),
          _withoutScope( std::move( other._withoutScope ) ),
          _isUniqueLocal( other._isUniqueLocal ),
          _isSiteLocal( other._isSiteLocal ),
          _isV4Mapped( other._isV4Mapped ),
          _isV4Compatible( other._isV4Compatible ),
          _isV4Translated( other._isV4Translated ),
          _is6to4( other._is6to4 ),
          _multicastFlags( other._multicastFlags )
    {
        other._isUniqueLocal = false;
        other._isSiteLocal = false;
        other._isV4Mapped = false;
        other._isV4Compatible = false;
        other._isV4Translated = false;
        other._is6to4 = false;
        other._multicastFlags = 0;
    }

    OddSource_Inline
    IPv6Address::
    ~IPv6Address() noexcept // NOLINT(*-use-equals-default)
    {
    }

    OddSource_Inline
    IPv6Address::
    operator in6_addr const *() const
    {
        return this->_data.get();
    }

    OddSource_Inline
    IPv6Address::
    operator Bytes() const
    {
        Bytes result{};
        ::std::memcpy( result.data(), this->_data->s6_addr, 16 );
        return result;
    }

    OddSource_Inline
    IPv6Address
    IPv6Address::
    normalize() const
    {
        if ( this->_scope )
        {
            return { this->_data.get(), *this->_scope };
        }
        return IPv6Address( this->_data.get() );
    }

    OddSource_Inline
    bool
    IPv6Address::
    isUniqueLocal() const
    {
        return this->_isUniqueLocal;
    }

    OddSource_Inline
    bool
    IPv6Address::
    isSiteLocal() const
    {
        return this->_isSiteLocal;
    }

    OddSource_Inline
    bool
    IPv6Address::
    isV4Mapped() const
    {
        return this->_isV4Mapped;
    }

    OddSource_Inline
    bool
    IPv6Address::
    isV4Translated() const
    {
        return this->_isV4Translated;
    }

    OddSource_Inline
    bool
    IPv6Address::
    isV4Compatible() const
    {
        return this->_isV4Compatible;
    }

    OddSource_Inline
    bool
    IPv6Address::
    is6to4() const
    {
        return this->_is6to4;
    }

    OddSource_Inline
    bool
    IPv6Address::
    hasScopeId() const
    {
        return static_cast< bool >( this->_scope );
    }

    OddSource_Inline
    ::std::string
    IPv6Address::
    withoutScopeId() const
    {
        return this->_withoutScope;
    }

    OddSource_Inline
    ::std::optional< ::std::uint32_t > const &
    IPv6Address::
    scopeId() const
    {
        static constexpr ::std::optional< ::std::uint32_t > nil; // prevent "returning ref to temp local"
        return this->_scope ? this->_scope->scopeId : nil;
    }

    OddSource_Inline
    ::std::optional< ::std::string > const &
    IPv6Address::
    scopeName() const
    {
        static ::std::optional< ::std::string > const nil; // prevent "returning ref to temp local"
        return this->_scope ? this->_scope->scopeName : nil;
    }

    OddSource_Inline
    ::std::optional< ::std::string >
    IPv6Address::
    scopeNameOrId() const
    {
        if ( !this->_scope )
        {
            return ::std::nullopt;
        }
        return this->_scope->scopeName ? this->_scope->scopeName : ::std::to_string( *this->_scope->scopeId );
    }

    OddSource_Inline
    ::std::optional< ::std::string >
    IPv6Address::
    scopeIdOrName() const
    {
        if ( !this->_scope )
        {
            return ::std::nullopt;
        }
        return this->_scope->scopeId ? ::std::to_string( *this->_scope->scopeId ) : this->_scope->scopeName;
    }

    OddSource_Inline
    bool
    IPv6Address::
    isMulticastFlagEnabled(
        MulticastV6Flag const & flag ) const
    {
        return this->_multicastFlags && ( *this->_multicastFlags & flag ) == flag;
    }

    OddSource_Inline
    bool
    IPv6Address::
    operator==(
        IPv6Address const & other ) const
    {
        auto const data1( reinterpret_cast< ::std::uint8_t const * >( this->_data.get() ) );
        auto const data2( reinterpret_cast< ::std::uint8_t const * >( other._data.get() ) );
        auto const length = this->dataLength();
        for( size_t i{ 0 }; i < length; ++i)
        {
            if ( data1[ i ] != data2[ i ] )
            {
                return false;
            }
        }
        return true;
    }

    OddSource_Inline
    bool
    IPv6Address::
    operator!=(
        IPv6Address const & other ) const
    {
        return !this->operator==(other);
    }

    OddSource_Inline
    ::std::string
    toString(
        IPAddress const & address )
    {
        return address.operator::std::string();;
    }

    OddSource_Inline
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        IPAddress const & address )
    {
        return os << address.operator::std::string();
    }
} // end namespace OddSource::Interfaces

#undef BYTES
#undef WORDS
#undef DOUBLEWORDS
