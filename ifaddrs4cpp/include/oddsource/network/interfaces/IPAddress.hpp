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

#ifndef ODDSOURCE_NETWORK_INTERFACES_IPADDRESS_HPP
#define ODDSOURCE_NETWORK_INTERFACES_IPADDRESS_HPP

#include "detail/bitwise_enum.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "detail/config.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "detail/winsock_includes.h"

#ifndef ODDSOURCE_IS_WINDOWS
#include <netinet/in.h>
#endif /* !ODDSOURCE_IS_WINDOWS */

#include <array>
#include <memory>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

#ifdef IFADDRS4CPP_INCLUDE_BOOST
#  include ODDSOURCE_BOOST_HEADER(asio/ip/address.hpp)
#endif /* IFADDRS4CPP_INCLUDE_BOOST */

namespace OddSource::Interfaces
{
#ifdef ODDSOURCE_IS_WINDOWS
    typedef USHORT AddressFamily;
#else /* ODDSOURCE_IS_WINDOWS */
    typedef sa_family_t AddressFamily;
#endif /* ODDSOURCE_IS_WINDOWS */

    class OddSource_Export InvalidIPAddress : public ::std::invalid_argument
    {
    public:
        OddSource_Inline
        explicit
        InvalidIPAddress(
            ::std::string_view const & what );

        OddSource_Inline
        InvalidIPAddress(
            InvalidIPAddress const & other );

        OddSource_Inline
        virtual
        ~InvalidIPAddress() noexcept; // NOLINT(*-use-override)
    };

    enum class OddSource_Export IPAddressVersion : ::std::uint16_t
    {
        IPv4 = 4,
        IPv6 = 6
    };

    [[nodiscard]]
    OddSource_Export
    ::std::string
    toString(
        IPAddressVersion const & version );

    OddSource_Export
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        IPAddressVersion const & version );

    enum class OddSource_Export MulticastScope : ::std::uint16_t
    {
        Reserved, // v6 only
        InterfaceLocal, // v6 only
        LinkLocal,
        RealmLocal,
        AdminLocal, // v6 only
        SiteLocal, // v6 only
        OrganizationLocal,
        Global,
        Unassigned
    };

#ifndef IFADDRS4CPP_INLINE_SOURCE
    OddSource_Extern OddSource_Export
    ::std::unordered_map< ::std::string, MulticastScope const > const
    MulticastScope_Values;

    OddSource_Extern OddSource_Export
    ::std::unordered_map< MulticastScope, ::std::string const > const
    MulticastScope_Names;
#endif /* !IFADDRS4CPP_INLINE_SOURCE */

    [[nodiscard]]
    OddSource_Export
    ::std::string
    toString(
        MulticastScope const & scope );

    OddSource_Export
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        MulticastScope const & scope );

    enum class OddSource_Export MulticastV6Flag : ::std::uint8_t
    {
        DynamicallyAssigned = 0b0001,
        PrefixBased = 0b0010,
        RendezvousEmbedded = 0b0100,
        ReservedFlag = 0b1000 // unused, here for clarity and unit testing
    };

    ODDSOURCE_BITWISE_ENUM_OPERATORS_DECL( MulticastV6Flag );

    class OddSource_Export IPAddress
    {
    public:
        virtual
        OddSource_Inline
        ~IPAddress() noexcept;

        [[nodiscard]]
#if __cplusplus >= 202002L
        constexpr
#endif /* __cplusplus >= 202002L */
        virtual
        IPAddressVersion
        version() const noexcept = 0;

        [[nodiscard]]
#if __cplusplus >= 202002L
        constexpr
#endif /* __cplusplus >= 202002L */
        virtual
        ::std::uint16_t
        maximumPrefixLength() const noexcept = 0;

        [[nodiscard]]
#if __cplusplus >= 202002L
        constexpr
#endif /* __cplusplus >= 202002L */
        virtual
        size_t
        dataLength() const noexcept = 0;

#if defined( IFADDRS4CPP_INCLUDE_BOOST ) && ( !defined( ODDSOURCE_IS_WINDOWS ) || defined( ODDSOURCE_BUILDING_STATIC_LIBRARY ) )
        [[nodiscard]]
        virtual
        explicit
        operator IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address() const = 0;
#endif /* IFADDRS4CPP_INCLUDE_BOOST */

        /**
         * Converts the IP address to a C++ string.
         *
         * @return a string.
         */
        [[nodiscard]]
        virtual
        OddSource_Inline
        explicit
        operator ::std::string() const;

        /**
         * Converts the IP address to a C string.
         *
         * @return a string.
         */
        [[nodiscard]]
        virtual
        OddSource_Inline
        explicit
        operator char const *() const;

        /**
         * Indicates whether this address represents the unspecified address
         * (0.0.0.0 or ::0).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        isUnspecified() const;

        /**
         * Indicates whether this address represents the loopback address
         * (127.0.0.0/8 or ::1).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        isLoopback() const;

        /**
         * Indicates whether this address represents a link-local address
         * (169.254.0.0/16 or fe80::/64).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        isLinkLocal() const;

        /**
         * Indicates whether this address represents a "private" address,
         * which has different meanings for IPv4 vs IPv6. IPv4 address
         * consider several ranges private (10.0.0.0/8, 100.64.0.0/10,
         * 172.16.0.0/12, 192.0.0.0/24, 192.168.0.0/16, and 198.18.0.0/15),
         * while IPv6 addresses specifically differentiate between
         * site-local (deprecated, fec0::/10) and unique-local (fc00::/7),
         * though both are often considered "private."
         *
         * @return
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        isPrivate() const;

        /**
         * Indicates whether this address represents a multicast address
         * (224.0.0.0/4 or ff00::/8).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        isMulticast() const;

        /**
         * Indicates whether this address represents any IANA-reserved
         * address, which could overlap with other flags, such as
         * isLinkLocal, isPrivate, isMulticast, etc. See
         * https://en.wikipedia.org/wiki/Reserved_IP_addresses for more
         * information about which other addresses are considered "reserved"
         * that don't already have their own is* methods here. Note: An
         * IP address's being reserved does not indicate that it cannot be
         * used.
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        isReserved() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::optional< MulticastScope > const &
        multicastScope() const;

        [[nodiscard]]
        static
        OddSource_Inline
        ::std::unique_ptr< IPAddress >
        create(
        ::std::string_view repr );

    protected:
        OddSource_Inline
        explicit
        IPAddress(
            ::std::string_view const & repr );

        // copy constructor
        OddSource_Inline
        IPAddress(
            IPAddress const & other );

        // move constructor
        OddSource_Inline
        IPAddress(
            IPAddress && other ) noexcept;

        OddSource_Inline
        IPAddress &
        operator=(
            IPAddress const & rhs );

        OddSource_Inline
        IPAddress &
        operator=(
            IPAddress && rhs ) noexcept;

        ::std::string _representation;
        bool _isUnspecified = false;
        bool _isLoopback = false;
        bool _isLinkLocal = false;
        bool _isPrivate = false;
        bool _isMulticast = false;
        bool _isReserved = false;
        ::std::optional< MulticastScope > _multicastScope;
    };

    class OddSource_Export IPv4Address : public IPAddress
    {
    public:
        IPv4Address() = delete;

        // conversion constructor
        OddSource_Inline
        explicit
        IPv4Address(
            ::std::string_view const & repr );

        // conversion constructor
        OddSource_Inline
        explicit
        IPv4Address(
            in_addr const * data );

        OddSource_Inline
        explicit
        IPv4Address(
            ::std::uint32_t data );

#ifdef IFADDRS4CPP_INCLUDE_BOOST
#  if !defined( ODDSOURCE_IS_WINDOWS ) || defined( ODDSOURCE_BUILDING_STATIC_LIBRARY )
        inline
        explicit
        IPv4Address(
            IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address const & other );

        inline
        explicit
        IPv4Address(
            IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4 const & other );
#  else
        template< class A, ::std::enable_if_t<
            ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address >,
            bool > = true >
        explicit
        IPv4Address(
            A const & other );

        template< class A, ::std::enable_if_t<
            ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4 >,
            bool > = true >
        explicit
        IPv4Address(
            A const & other );
#  endif
#endif /* IFADDRS4CPP_INCLUDE_BOOST */

        // copy constructor
        OddSource_Inline
        IPv4Address(
            IPv4Address const & );

        // move constructor
        OddSource_Inline
        IPv4Address(
            IPv4Address && ) noexcept;

        virtual
        OddSource_Inline
        ~IPv4Address() noexcept; // NOLINT(*-use-override)[[nodiscard]]

        OddSource_Inline
        IPv4Address &
        operator=(
            IPv4Address const & rhs );

        OddSource_Inline
        IPv4Address &
        operator=(
            IPv4Address && rhs ) noexcept;

        [[nodiscard]]
        inline
#if __cplusplus >= 202002L
        constexpr
#endif /* __cplusplus >= 202002L */
        IPAddressVersion
        version() const noexcept override;

        [[nodiscard]]
        inline
#if __cplusplus >= 202002L
        constexpr
#endif /* __cplusplus >= 202002L */
        ::std::uint16_t
        maximumPrefixLength() const noexcept override;

        [[nodiscard]]
        inline
#if __cplusplus >= 202002L
        constexpr
#endif /* __cplusplus >= 202002L */
        size_t
        dataLength() const noexcept override;

#ifdef IFADDRS4CPP_INCLUDE_BOOST
#  if !defined( ODDSOURCE_IS_WINDOWS ) || defined( ODDSOURCE_BUILDING_STATIC_LIBRARY )
        [[nodiscard]]
        virtual
        inline
        explicit
        operator IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address() const override;

        [[nodiscard]]
        inline
        explicit
        operator IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4() const;
#  else
        [[nodiscard]]
        template< class A, ::std::enable_if_t<
            ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address >,
            bool > = true >
        inline
        explicit
        operator A() const override;

        [[nodiscard]]
        template< class A, ::std::enable_if_t<
            ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4 >,
            bool > = true >
        inline
        explicit
        operator A() const;
#  endif
#endif /* IFADDRS4CPP_INCLUDE_BOOST */

        [[nodiscard]]
        OddSource_Inline
        explicit
        operator in_addr const *() const;

        [[nodiscard]]
        OddSource_Inline
        explicit
        operator ::std::uint32_t() const;

        [[nodiscard]]
        OddSource_Inline
        bool
        operator==(
            IPv4Address const & ) const;

        [[nodiscard]]
        OddSource_Inline
        bool
        operator!=(
            IPv4Address const & ) const;

    private:
        OddSource_Inline
        explicit
        IPv4Address(
            ::std::unique_ptr< in_addr const > && );

        ::std::unique_ptr< in_addr const > _data;
    };

    struct OddSource_Export v6Scope
    {
        ::std::optional< ::std::uint32_t > scopeId = ::std::nullopt;
        ::std::optional< ::std::string > scopeName = ::std::nullopt;
    };

    class OddSource_Export IPv6Address : public IPAddress
    {
    public:
        using Bytes = ::std::array< ::std::uint8_t, 16 >;

        IPv6Address() = delete;

        // conversion constructor
        OddSource_Inline
        explicit
        IPv6Address(
            ::std::string_view const & repr );

        // conversion constructor
        OddSource_Inline
        explicit
        IPv6Address(
            in6_addr const * data );

        OddSource_Inline
        IPv6Address(
            in6_addr const * data,
            ::std::uint32_t scopeId );

        OddSource_Inline
        IPv6Address(
            in6_addr const * data,
            ::std::string_view const & scopeName );

        OddSource_Inline
        IPv6Address(
            in6_addr const * data,
            v6Scope const & scope );

        OddSource_Inline
        explicit
        IPv6Address(
            Bytes const & data );

        OddSource_Inline
        IPv6Address(
            Bytes const & data,
            ::std::uint32_t scopeId );

        OddSource_Inline
        IPv6Address(
            Bytes const & data,
            ::std::string_view const & scopeName );

        OddSource_Inline
        IPv6Address(
            Bytes const & data,
            v6Scope const & scope );

#ifdef IFADDRS4CPP_INCLUDE_BOOST
#  if !defined( ODDSOURCE_IS_WINDOWS ) || defined( ODDSOURCE_BUILDING_STATIC_LIBRARY )
        inline
        explicit
        IPv6Address(
            IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address const & other );

        inline
        explicit
        IPv6Address(
            IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6 const & other );
#  else
        template< class A, ::std::enable_if_t<
            ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address >,
            bool > = true >
        explicit
        IPv6Address(
            A const & other );

        template< class A, ::std::enable_if_t<
            ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6 >,
            bool > = true >
        explicit
        IPv6Address(
            A const & other );
#  endif
#endif /* IFADDRS4CPP_INCLUDE_BOOST */

        // copy constructor
        OddSource_Inline
        IPv6Address(
            IPv6Address const & other );

        // move constructor
        OddSource_Inline
        IPv6Address(
            IPv6Address && other ) noexcept;

        virtual
        OddSource_Inline
        ~IPv6Address() noexcept; // NOLINT(*-use-override)

        OddSource_Inline
        IPv6Address &
        operator=(
            IPv6Address const & rhs );

        OddSource_Inline
        IPv6Address &
        operator=(
            IPv6Address && rhs ) noexcept;

        [[nodiscard]]
        inline
#if __cplusplus >= 202002L
        constexpr
#endif /* __cplusplus >= 202002L */
        IPAddressVersion
        version() const noexcept override;

        [[nodiscard]]
        inline
#if __cplusplus >= 202002L
        constexpr
#endif /* __cplusplus >= 202002L */
        ::std::uint16_t
        maximumPrefixLength() const noexcept override;

        [[nodiscard]]
        inline
#if __cplusplus >= 202002L
        constexpr
#endif /* __cplusplus >= 202002L */
        size_t
        dataLength() const noexcept override;

#ifdef IFADDRS4CPP_INCLUDE_BOOST
#  if !defined( ODDSOURCE_IS_WINDOWS ) || defined( ODDSOURCE_BUILDING_STATIC_LIBRARY )
        [[nodiscard]]
        virtual
        inline
        explicit
        operator IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address() const override;

        [[nodiscard]]
        inline
        explicit
        operator IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6() const;
#  else
        [[nodiscard]]
        template< class A, ::std::enable_if_t<
            ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address >,
            bool > = true >
        inline
        explicit
        operator A() const override;

        [[nodiscard]]
        template< class A, ::std::enable_if_t<
            ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6 >,
            bool > = true >
        inline
        explicit
        operator A() const;
#  endif
#endif /* IFADDRS4CPP_INCLUDE_BOOST */

        [[nodiscard]]
        OddSource_Inline
        explicit
        operator in6_addr const *() const;

        [[nodiscard]]
        OddSource_Inline
        explicit
        operator Bytes() const;

        /**
         * Returns a copy of this address with a normalized string representation.
         * For example, an address with multiple consecutive zeros, such as
         * 2001:0:0:0:de:ad:be:ef, would be normalized to 2001::de:ad:be:ef. As
         * another example, a v4-translated address, such as ::ffff:0:127.0.0.1,
         * would be normalized to ::ffff:0:7f00:1. However, v4-mapped and
         * v4-compatible addresses do not similarly change representation when
         * normalized. As part of normalization, all characters become lowercase.
         *
         * @return a normalized copy of this address.
         */
        [[nodiscard]]
        OddSource_Inline
        IPv6Address
        normalize() const;

        /**
         * Indicates whether this address represents a unique-local address,
         * (fc00::/7).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        isUniqueLocal() const;

        /**
         * Indicates whether this address represents a deprecated site-local
         * address (fec0::/10).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        isSiteLocal() const;

        /**
         * Indicates whether this address represents an IPv4-mapped address
         * (::ffff:0:0/96, i.e. ::ffff:0.0.0.0 - ::ffff:255.255.255.255).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        isV4Mapped() const;

        /**
         * Indicates whether this address represents an IPv4-translated address
         * (::ffff:0:0:0/96, i.e. ::ffff:0:0.0.0.0 - ::ffff:0:255.255.255.255).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        isV4Translated() const;

        /**
         * Indicates whether this address represents a deprecated IPv4-compatible
         * address (::0/96 from ::2, i.e. ::0.0.0.2 - ::255.255.255.255). These
         * are deprecated for numerous reasons, one of which is that addresses
         * 0.0.0.0-0.0.0.1 cannot be represented.
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        isV4Compatible() const;

        /**
         * Indicates whether this address represents a deprecated 6to4 address
         * (2002::/16, i.e. 2002:: - 2002:ffff:ffff:ffff:ffff:ffff:ffff:ffff).
         *
         * @return true or false;
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        is6to4() const;

        [[nodiscard]]
        OddSource_Inline
        bool
        hasScopeId() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::string
        withoutScopeId() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::optional<::std::uint32_t> const &
        scopeId() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::optional<::std::string> const &
        scopeName() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::optional<::std::string>
        scopeNameOrId() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::optional<::std::string>
        scopeIdOrName() const;

        [[nodiscard]]
        OddSource_Inline
        bool
        isMulticastFlagEnabled(
            MulticastV6Flag const & flag ) const;

        [[nodiscard]]
        OddSource_Inline
        bool
        operator==(
            IPv6Address const & ) const;

        [[nodiscard]]
        OddSource_Inline
        bool
        operator!=(
            IPv6Address const & ) const;

    private:
        OddSource_Inline
        IPv6Address(
            ::std::string_view const & reprWithScope,
            ::std::string_view const & reprWithoutScope );

        OddSource_Inline
        IPv6Address(
            ::std::unique_ptr< in6_addr const > && data,
            ::std::optional< v6Scope > && scope );

        OddSource_Inline
        IPv6Address(
            ::std::string && reprWithoutScope,
            ::std::unique_ptr< in6_addr const > && data,
            ::std::optional< v6Scope > && scope );

        ::std::unique_ptr< in6_addr const > _data;
        ::std::optional< v6Scope > _scope;
        ::std::string _withoutScope;
        bool _isUniqueLocal = false;
        bool _isSiteLocal = false;
        bool _isV4Mapped = false;
        bool _isV4Compatible = false;
        bool _isV4Translated = false;
        bool _is6to4 = false;
        ::std::optional< ::std::uint8_t > _multicastFlags;
    };

    [[nodiscard]]
    OddSource_Export
    ::std::string
    toString(
        IPAddress const & address );

    OddSource_Export
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        IPAddress const & address );
}

#include "detail/IPAddress.hpp"

#ifdef IFADDRS4CPP_INLINE_SOURCE
#include "impl/IPAddress.ipp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

#endif /* ODDSOURCE_NETWORK_INTERFACES_IPADDRESS_HPP */
