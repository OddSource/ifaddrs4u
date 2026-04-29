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

#include "../detail/flip.hpp"

#ifdef ODDSOURCE_IS_WINDOWS
#include "winsock_includes.h"
#else /* ODDSOURCE_IS_WINDOWS */
#include <arpa/inet.h>
#include <cerrno>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif /* ODDSOURCE_IS_WINDOWS */

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
    using Enable_If_Addr = ::std::enable_if_t< ::std::is_same_v<Addr, in_addr> ||
                                               ::std::is_same_v<Addr, in6_addr> >;

    template< typename Addr, typename = Enable_If_Addr<Addr> >
    ::std::unique_ptr< Addr >
    fromRepr(
        ::std::string_view const & repr )
    {
        using namespace ::std::string_literals;
        if ( repr.empty() )
        {
            throw InvalidIPAddress("Invalid empty IP address string.");
        }

        ::std::string repr_str(repr);
        auto data = ::std::make_unique<Addr>();
        int success;
        if constexpr (::std::is_same_v<Addr, in6_addr>)
        {
            // inet_pton can also handle IPv4 addresses, but only in dotted-decimal format
            // (1.2.3.4), not in octal, hexadecimal, or any other valid IPv4 format.
            success = inet_pton(AF_INET6, repr_str.c_str(), data.get());
        }
        else
        {
            int num_dots(0);
            for (char c : repr)
            {
                if (c == '.')
                {
                    num_dots++;
                }
            }
            if (num_dots != 3)
            {
                // some implementations of inet_aton tolerate incomplete addresses, but we do not
                throw InvalidIPAddress(
                        "Malformed IPv4 address string '"s + repr_str + "' with "s +
                        ::std::to_string(num_dots + 1) + " parts instead of 4"s);
            }
            // inet_aton/RtlIpv4StringToAddress, however, can handle IPv4 addresses in all valid formats.
#ifdef ODDSOURCE_IS_WINDOWS
            char const * end = nullptr;
            success = RtlIpv4StringToAddress(repr_str.c_str(), false, &end, data.get());
            if (success == STATUS_INVALID_PARAMETER)
            {
                throw InvalidIPAddress(
                    "An invalid parameter was passed to RtlIpv4StringToAddress while converting '"s +
                    repr_str + "'"s);
            }
            else if (success != 0)
            {
                char * s = nullptr;
                ::FormatMessage(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    nullptr, success,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (LPTSTR)&s, 0, nullptr);
                ::std::string err(s == nullptr ? "" : s);
                LocalFree(s);
                throw InvalidIPAddress(
                    "Malformed IP address string '"s + repr_str + "' or unknown RtlIpv4StringToAddress error ("s +
                    ::std::to_string(success) + "): "s + err);
            }
            success = 1;
#else /* ODDSOURCE_IS_WINDOWS */
            success = inet_aton(repr_str.c_str(), data.get());
#endif /* !ODDSOURCE_IS_WINDOWS */
        }
        if (success != 1)
        {
            throw InvalidIPAddress("Malformed IP address string '"s + repr_str + "' or unknown inet_*ton error."s);
        }

        return data;
    }

    template< typename Addr, typename = Enable_If_Addr< Addr > >
    ::std::string
    toRepr(
        Addr const * data )
    {
        using namespace ::std::string_literals;
        AddressFamily family;
        if constexpr (::std::is_same_v<Addr, in6_addr>)
        {
            family = AF_INET6;
        }
        else
        {
            family = AF_INET;
        }

        static constexpr size_t host_length(100);
        char host_chars[host_length];
        auto ptr = ::inet_ntop(family, data, host_chars, host_length);
        if (ptr == nullptr)
        {
#ifdef ODDSOURCE_IS_WINDOWS
            auto err_no(::WSAGetLastError());
            char * s = nullptr;
            ::FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                nullptr, err_no,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&s, 0, nullptr);
            ::std::string err(s == nullptr ? "" : s);
            LocalFree(s);
#else /* ODDSOURCE_IS_WINDOWS */
            auto err_no(errno);
            char const * err(
                    err_no == EAFNOSUPPORT ? "Address family not supported" :
                    (err_no == ENOSPC ? "Converted address would exceed string size" :
                     ::gai_strerror(errno)));
#endif /* !ODDSOURCE_IS_WINDOWS */
            throw InvalidIPAddress(
                "Malformed in_addr data or inet_ntop system error: "s +
                ::std::to_string(err_no) + " ('"s + err + "')"s);
        }
        return host_chars;
    }

    template< typename Addr, typename = Enable_If_Addr< Addr > >
    ::std::string
    toRepr(
        ::std::unique_ptr< Addr const > const & data )
    {
        return toRepr(data.get());
    }

    template< typename Addr, typename = Enable_If_Addr< Addr > >
    ::std::unique_ptr< Addr >
    copy_in_addr(
        Addr const * data )
    {
        auto new_data = ::std::make_unique<Addr>();
        ::std::memcpy(new_data.get(), data, sizeof(Addr));
        return new_data;
    }

    template< typename Addr, typename = Enable_If_Addr< Addr > >
    ::std::unique_ptr< Addr >
    copy_in_addr(
        ::std::unique_ptr< Addr const > const & data )
    {
        return copy_in_addr(data.get());
    }

    v6Scope &&
    fillOutScope(
        v6Scope && scope )
    {
        if (scope.scope_id && !scope.scope_name)
        {
            char buffer[IF_NAMESIZE];
            if (::if_indextoname(*scope.scope_id, buffer) != nullptr)
            {
                scope.scope_name = ::std::string(buffer);
            }
        }
        else if(scope.scope_name && !scope.scope_id)
        {
            ::std::uint32_t scope_id = ::if_nametoindex(scope.scope_name->c_str());
            if (scope_id > 0)
            {
                scope.scope_id = scope_id;
            }
        }
        return std::move( scope );
    }

    v6Scope
    scopeFrom(
        ::std::uint32_t scope_id )
    {
        if (scope_id == 0)
        {
            throw ::std::invalid_argument("IPv6 address scope ID must be greater than 0.");
        }
        return fillOutScope( { scope_id } );
    }

    v6Scope
    scopeFrom(
        ::std::string_view const & scope_name )
    {
        if (scope_name.empty())
        {
            throw ::std::invalid_argument("IPv6 address scope name must not be an empty string.");
        }
        return fillOutScope( { ::std::nullopt, ::std::string(scope_name) } );
    }

    ::std::string_view
    stripScope(
        ::std::string_view const & repr )
    {
        size_t const i = repr.find('%');
        if (i != ::std::string_view::npos)
        {
            return repr.substr(0, i);
        }
        return repr;
    }

    ::std::optional< v6Scope >
    extractScope(
        ::std::string_view const & repr )
    {
        size_t const i = repr.find('%');
        if (i != ::std::string_view::npos)
        {
            ::std::string const scope(repr.substr(i + 1));
            if (!scope.empty())
            {
                if (scope.find_first_not_of("0123456789") == std::string::npos)
                {
                    try
                    {
                        return scopeFrom(::std::stoul(scope));
                    }
                    catch (::std::invalid_argument const &)
                    {
                    }
                }
                return scopeFrom(scope);
            }
        }
        return ::std::nullopt;
    }

    ::std::string
    addScope(
        ::std::string const & repr,
        ::std::optional< v6Scope > const & scope )
    {
        if (!scope)
        {
            return repr;
        }
        return repr + "%" + (scope->scope_name ? *scope->scope_name : ::std::to_string(*scope->scope_id));
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

    OddSource_Inline
    ::std::underlying_type_t< MulticastV6Flag >
    operator&(
        ::std::underlying_type_t< MulticastV6Flag > lhs,
        MulticastV6Flag const & rhs )
    {
        return lhs & static_cast< ::std::underlying_type_t< MulticastV6Flag > >( rhs );
    }

    OddSource_Inline
    ::std::underlying_type_t< MulticastV6Flag >
    operator|(
        MulticastV6Flag const & lhs,
        MulticastV6Flag const & rhs )
    {
        return static_cast< ::std::underlying_type_t< MulticastV6Flag > >( lhs ) |
               static_cast< ::std::underlying_type_t< MulticastV6Flag > >( rhs );
    }

    OddSource_Export
    ::std::underlying_type_t< MulticastV6Flag >
    operator|(
        ::std::underlying_type_t< MulticastV6Flag > lhs,
        MulticastV6Flag const & rhs )
    {
        return lhs | static_cast< ::std::underlying_type_t< MulticastV6Flag > >( rhs );
    }

    OddSource_Inline
    bool
    operator==(
        ::std::underlying_type_t< MulticastV6Flag > lhs,
        MulticastV6Flag const & rhs )
    {
        return lhs == static_cast< ::std::underlying_type_t< MulticastV6Flag > >( rhs );
    }

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
          _is_unspecified( other._is_unspecified ),
          _is_loopback( other._is_loopback ),
          _is_link_local( other._is_link_local ),
          _is_private( other._is_private ),
          _is_multicast( other._is_multicast ),
          _is_reserved( other._is_reserved ),
          _multicast_scope( other._multicast_scope )
    {
    }

    OddSource_Inline
    IPAddress::
    IPAddress(
        IPAddress && other ) noexcept
        : _representation( std::move( other._representation ) ),
          _is_unspecified(other._is_unspecified),
          _is_loopback(other._is_loopback),
          _is_link_local(other._is_link_local),
          _is_private(other._is_private),
          _is_multicast(other._is_multicast),
          _is_reserved(other._is_reserved),
          _multicast_scope( ::std::move( other._multicast_scope ) ) // NOLINT(*-move-const-arg)
    {
        other._is_unspecified = false;
        other._is_loopback = false;
        other._is_link_local = false;
        other._is_private = false;
        other._is_multicast = false;
        other._is_reserved = false;
    }

    OddSource_Inline
    IPAddress::
    ~IPAddress() noexcept // NOLINT(*-use-equals-default)
    {
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
    is_unspecified() const
    {
        return this->_is_unspecified;
    }

    OddSource_Inline
    bool
    IPAddress::
    is_loopback() const
    {
        return this->_is_loopback;
    }

    OddSource_Inline
    bool
    IPAddress::
    is_link_local() const
    {
        return this->_is_link_local;
    }

    OddSource_Inline
    bool
    IPAddress::
    is_private() const
    {
        return this->_is_private;
    }

    OddSource_Inline
    bool
    IPAddress::
    is_multicast() const
    {
        return this->_is_multicast;
    }

    OddSource_Inline
    bool
    IPAddress::
    is_reserved() const
    {
        return this->_is_reserved;
    }

    OddSource_Inline
    ::std::optional< MulticastScope > const &
    IPAddress::
    multicast_scope() const
    {
        return this->_multicast_scope;
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
        ::std::unique_ptr< in_addr const > && data )
        : IPAddress( toRepr( data ) ),
          _data( ::std::move( data ) )
    {
        auto const bytes = BYTES;

        if (*reinterpret_cast<::std::uint32_t const *>(this->_data.get()) == 0)
        {
            this->_is_unspecified = true;
            this->_is_reserved = true;
        }
        else if (bytes[0] == 127) // 127.0.0.0/8
        {
            this->_is_loopback = true;
            this->_is_reserved = true;
        }
        else if (bytes[0] == 169 && bytes[1] == 254) // 169.254.0.0/16
        {
            this->_is_link_local = true;
            this->_is_reserved = true;
        }
        else if (bytes[0] == 10 || // 10.0.0.0/8
                 (bytes[0] == 100 && bytes[1] >= 64 && bytes[1] <= 127) || // 100.64.0.0/10
                 (bytes[0] == 172 && bytes[1] >= 16 && bytes[1] <= 31) || // 172.16.0.0/12
                 (bytes[0] == 192 && bytes[1] == 0 && bytes[2] == 0) || // 192.0.0.0/24
                 (bytes[0] == 192 && bytes[1] == 168) || // 192.168.0.0/16
                 (bytes[0] == 198 && bytes[1] >= 18 && bytes[1] <= 19) // 198.18.0.0/15
                )
        {
            this->_is_private = true;
            this->_is_reserved = true;
        }
        else if (bytes[0] >= 224 && bytes[0] <= 239) // 224.0.0.0/4
        {
            this->_is_multicast = true;
            this->_is_reserved = true;
        }
        // various other reserved ranges, see https://en.wikipedia.org/wiki/Reserved_IP_addresses
        else if (bytes[0] == 0 || // 0.0.0.0/8
                 (bytes[0] == 192 && bytes[1] == 0 && bytes[2] == 2) || // 192.0.2.0/24
                 (bytes[0] == 192 && bytes[1] == 88 && bytes[2] == 99) || // 192.88.99.0/24
                 (bytes[0] == 198 && bytes[1] == 51 && bytes[2] == 100) || // 198.51.100.0/24
                 (bytes[0] == 203 && bytes[1] == 0 && bytes[2] == 113) || // 203.0.113.0/24
                 (bytes[0] == 233 && bytes[1] == 252 && bytes[2] == 0) || // 233.252.0.0/24
                 bytes[0] >= 240 // 240.0.0.0/4
                )
        {
            this->_is_reserved = true;
        }

        if (this->is_multicast())
        {
            if (bytes[0] == 224 && bytes[1] == 0 && bytes[2] == 0) // 224.0.0.0/24
            {
                this->_multicast_scope = MulticastScope::LinkLocal;
            }
            else if (bytes[0] == 239 && bytes[1] == 255) // 239.255.0.0/16
            {
                this->_multicast_scope = MulticastScope::RealmLocal;
            }
            else if (bytes[0] == 239 && bytes[1] >= 192 && bytes[1] <= 195) // 239.192.0.0/14
            {
                this->_multicast_scope = MulticastScope::OrganizationLocal;
            }
            else if (bytes[0] != 239) // // 224.0.1.0-238.255.255.255
            {
                this->_multicast_scope = MulticastScope::Global;
            }
            else
            {
                this->_multicast_scope = MulticastScope::Unassigned;
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
    size_t
    IPv4Address::
    data_length() const
    {
        return 4;
    }

    OddSource_Inline
    IPv4Address::
    operator in_addr const *() const
    {
        return this->_data.get();
    }

    OddSource_Inline
    bool
    IPv4Address::
    operator==(
        IPv4Address const & other ) const
    {
        return (*reinterpret_cast<::std::uint32_t const *>(this->_data.get()) ==
                *reinterpret_cast<::std::uint32_t const *>(other._data.get()));
    }

    OddSource_Inline
    bool
    IPv4Address::
    operator!=(
        IPv4Address const & other ) const
    {
        return !this->operator==(other);
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
        : IPv6Address( toRepr( data ), copy_in_addr( data ), std::nullopt )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        in6_addr const * data,
        ::std::uint32_t scopeId )
        : IPv6Address( toRepr( data ), copy_in_addr( data ), scopeFrom( scopeId ) )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        in6_addr const * data,
        ::std::string_view const & scopeName )
        : IPv6Address( toRepr( data ), copy_in_addr( data ), scopeFrom( scopeName ) )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        in6_addr const * data,
        v6Scope const & scope )
        : IPv6Address( toRepr( data ), copy_in_addr( data ), scope )
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
        ::std::string && reprWithoutScope,
        ::std::unique_ptr< in6_addr const > && data,
        ::std::optional< v6Scope > && scope )
        : IPAddress( addScope( reprWithoutScope, scope ) ),
          _data( ::std::move( data ) ),
          _scope( std::move( scope ) ),
          _without_scope( reprWithoutScope )
    {
        auto const bytes = BYTES;
        auto const words = WORDS;
        auto const doublewords = DOUBLEWORDS;

        if (IN6_IS_ADDR_UNSPECIFIED(this->_data.get()))
        {
            this->_is_unspecified = true;
            this->_is_reserved = true;
        }
        else if(IN6_IS_ADDR_LOOPBACK(this->_data.get()))
        {
            this->_is_loopback = true;
            this->_is_reserved = true;
        }
        else if(IN6_IS_ADDR_LINKLOCAL(this->_data.get()) && // some impls erroneously check *only* fe80
                words[1] == 0 && words[2] == 0 && words[3] == 0)
        {
            this->_is_link_local = true;
            this->_is_reserved = true;
        }
        // IN6_IS_ADDR_UNIQUE_LOCAL is available only on BSD-based systems like macOS
        else if(bytes[0] == 0xfc || bytes[0] == 0xfd)
        {
            this->_is_unique_local = true;
            this->_is_private = true;
            this->_is_reserved = true;
        }
        else if(IN6_IS_ADDR_SITELOCAL(this->_data.get()))
        {
            this->_is_site_local = true;
            this->_is_private = true;
            this->_is_reserved = true;
        }
        else if(IN6_IS_ADDR_MULTICAST(this->_data.get()))
        {
            this->_is_multicast = true;
            this->_is_reserved = true;
        }
        else if(IN6_IS_ADDR_V4MAPPED(this->_data.get()))
        {
            this->_is_v4_mapped = true;
            this->_is_reserved = true;
        }
        else if(IN6_IS_ADDR_V4COMPAT(this->_data.get()))
        {
            this->_is_v4_compatible = true;
            this->_is_reserved = true;
        }
        // IN6_IS_ADDR_V4TRANSLATED is available only on Windows SDK / Winsock 2
        else if(doublewords[0] == 0 && doublewords[1] == 0 && words[4] == 0xffff && words[5] == 0)
        {
            this->_is_v4_translated = true;
            this->_is_reserved = true;
        }
        // IN6_IS_ADDR_6TO4 is available only on BSD-based systems like macOS
        else if(ntohs(words[0]) == 0x2002)
        {
            this->_is_6to4 = true;
            this->_is_reserved = true;
        }
        // various other reserved ranges, see https://en.wikipedia.org/wiki/Reserved_IP_addresses
        else if((ntohs(words[0]) == 0x64 && ntohs(words[1]) == 0xff9b && doublewords[1] == 0 && doublewords[2] == 0) || // 64:ff9b::/96
                (ntohs(words[0]) == 0x64 && ntohs(words[1]) == 0xff9b && doublewords[1] == 1) || // 64:ff9b:1::/48
                (ntohs(words[0]) == 0x100 && words[1] == 0 && doublewords[1] == 0) || // 100::/64
                (ntohs(words[0]) == 0x2001 && words[1] == 0) || // 2001:0000::/32
                (ntohs(words[0]) == 0x2001 && ntohs(words[1]) >= 0x20 && ntohs(words[1]) <= 0x2f) || // 2001:20::/28
                (ntohs(words[0]) == 0x2001 && ntohs(words[1]) == 0xdb8) // 2001:db8::/32
               )
        {
            this->_is_reserved = true;
        }

        if (this->is_multicast())
        {
            this->_multicast_flags = (bytes[1] & 0b11110000) >> 4;
            switch (bytes[1] & 0b1111)
            {
                case 0x0:
                case 0xf: this->_multicast_scope = MulticastScope::Reserved; break;
                case 0x1: this->_multicast_scope = MulticastScope::InterfaceLocal; break;
                case 0x2: this->_multicast_scope = MulticastScope::LinkLocal; break;
                case 0x3: this->_multicast_scope = MulticastScope::RealmLocal; break;
                case 0x4: this->_multicast_scope = MulticastScope::AdminLocal; break;
                case 0x5: this->_multicast_scope = MulticastScope::SiteLocal; break;
                case 0x8: this->_multicast_scope = MulticastScope::OrganizationLocal; break;
                case 0xe: this->_multicast_scope = MulticastScope::Global; break;
                default: this->_multicast_scope = MulticastScope::Unassigned; break;
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
          _without_scope( other._without_scope ),
          _is_unique_local( other._is_unique_local ),
          _is_site_local( other._is_site_local ),
          _is_v4_mapped( other._is_v4_mapped ),
          _is_v4_compatible( other._is_v4_compatible ),
          _is_v4_translated( other._is_v4_translated ),
          _is_6to4( other._is_6to4 ),
          _multicast_flags( other._multicast_flags )
    {
    }

    OddSource_Inline
    IPv6Address::
    IPv6Address(
        IPv6Address && other ) noexcept
        : IPAddress( std::move( other ) ),
          _data( copy_in_addr( other._data ) ),
          _scope( other._scope ),
          _without_scope( other._without_scope ),
          _is_unique_local( other._is_unique_local ),
          _is_site_local( other._is_site_local ),
          _is_v4_mapped( other._is_v4_mapped ),
          _is_v4_compatible( other._is_v4_compatible ),
          _is_v4_translated( other._is_v4_translated ),
          _is_6to4( other._is_6to4 ),
          _multicast_flags( other._multicast_flags )
    {
    }

    OddSource_Inline
    IPv6Address::
    ~IPv6Address() noexcept // NOLINT(*-use-equals-default)
    {
    }

    OddSource_Inline
    size_t
    IPv6Address::
    data_length() const
    {
        return 16;
    }

    OddSource_Inline
    IPv6Address::
    operator in6_addr const *() const
    {
        return this->_data.get();
    }

    OddSource_Inline
    IPv6Address
    IPv6Address::
    normalize() const
    {
        if (this->_scope)
        {
            return { this->_data.get(), *this->_scope };
        }
        return IPv6Address( this->_data.get() );
    }

    OddSource_Inline
    bool
    IPv6Address::
    is_unique_local() const
    {
        return this->_is_unique_local;
    }

    OddSource_Inline
    bool
    IPv6Address::
    is_site_local() const
    {
        return this->_is_site_local;
    }

    OddSource_Inline
    bool
    IPv6Address::
    is_v4_mapped() const
    {
        return this->_is_v4_mapped;
    }

    OddSource_Inline
    bool
    IPv6Address::
    is_v4_translated() const
    {
        return this->_is_v4_translated;
    }

    OddSource_Inline
    bool
    IPv6Address::
    is_v4_compatible() const
    {
        return this->_is_v4_compatible;
    }

    OddSource_Inline
    bool
    IPv6Address::
    is_6to4() const
    {
        return this->_is_6to4;
    }

    OddSource_Inline
    bool
    IPv6Address::
    has_scope_id() const
    {
        return static_cast< bool >( this->_scope );
    }

    OddSource_Inline
    ::std::string
    IPv6Address::
    without_scope_id() const
    {
        return this->_without_scope;
    }

    OddSource_Inline
    ::std::optional< ::std::uint32_t > const &
    IPv6Address::
    scope_id() const
    {
        static constexpr ::std::optional< ::std::uint32_t > nil; // prevent "returning ref to temp local"
        return this->_scope ? this->_scope->scope_id : nil;
    }

    OddSource_Inline
    ::std::optional< ::std::string > const &
    IPv6Address::
    scope_name() const
    {
        static ::std::optional<::std::string> const nil; // prevent "returning ref to temp local"
        return this->_scope ? this->_scope->scope_name : nil;
    }

    OddSource_Inline
    ::std::optional< ::std::string >
    IPv6Address::
    scope_name_or_id() const
    {
        if (!this->_scope)
        {
            return ::std::nullopt;
        }
        return this->_scope->scope_name ? this->_scope->scope_name : ::std::to_string(*this->_scope->scope_id);
    }

    OddSource_Inline
    ::std::optional< ::std::string >
    IPv6Address::
    scope_id_or_name() const
    {
        if (!this->_scope)
        {
            return ::std::nullopt;
        }
        return this->_scope->scope_id ? ::std::to_string(*this->_scope->scope_id) : this->_scope->scope_name;
    }

    OddSource_Inline
    bool
    IPv6Address::
    is_multicast_flag_enabled(
        MulticastV6Flag const & flag ) const
    {
        return this->_multicast_flags && ( *this->_multicast_flags & flag ) == flag;
    }

    OddSource_Inline
    bool
    IPv6Address::
    operator==(
        IPv6Address const & other ) const
    {
        auto data1 = reinterpret_cast<::std::uint8_t const *>(this->_data.get());
        auto data2 = reinterpret_cast<::std::uint8_t const *>(other._data.get());
        auto length = this->data_length();
        for(size_t i(0); i < length; i++)
        {
            if (data1[i] != data2[i])
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
        IPv6Address const & other) const
    {
        return !this->operator==(other);
    }

    OddSource_Export
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
