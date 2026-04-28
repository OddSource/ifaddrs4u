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

#ifndef ODDSOURCE_NETWORK_INTERFACES_IPADDRESS_HPP
#define ODDSOURCE_NETWORK_INTERFACES_IPADDRESS_HPP

#include "detail/config.h"
#include "detail/winsock_includes.h"

#ifndef ODDSOURCE_IS_WINDOWS
#include <netinet/in.h>
#endif /* !ODDSOURCE_IS_WINDOWS */

#include <memory>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>

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
        explicit InvalidIPAddress(::std::string_view const & what)
            : ::std::invalid_argument(::std::string(what)) {}
        InvalidIPAddress(InvalidIPAddress const & other) noexcept = default;
    };

    enum class OddSource_Export IPAddressVersion : ::std::uint8_t
    {
        IPv4 = 4,
        IPv6 = 6
    };

    OddSource_Export
    ::std::string
    toString(
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

    enum class OddSource_Export MulticastV6Flag : ::std::uint8_t
    {
        DynamicallyAssigned = 0b0001,
        PrefixBased = 0b0010,
        RendezvousEmbedded = 0b0100,
        ReservedFlag = 0b1000 // unused, here for clarity and unit testing
    };

    OddSource_Export
    ::std::underlying_type_t< MulticastV6Flag >
    operator&(
        ::std::underlying_type_t< MulticastV6Flag > lhs,
        MulticastV6Flag const & rhs );

    OddSource_Export
    ::std::underlying_type_t< MulticastV6Flag >
    operator|(
        MulticastV6Flag const & lhs,
        MulticastV6Flag const & rhs );

    OddSource_Export
    ::std::underlying_type_t< MulticastV6Flag >
    operator|(
        ::std::underlying_type_t< MulticastV6Flag > lhs,
        MulticastV6Flag const & rhs );

    OddSource_Export
    bool
    operator==(
        ::std::underlying_type_t< MulticastV6Flag > lhs,
        MulticastV6Flag const & rhs );

    class OddSource_Export IPAddress
    {
    public:
        virtual
        OddSource_Inline
        ~IPAddress() noexcept;

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
        is_unspecified() const;

        /**
         * Indicates whether this address represents the loopback address
         * (127.0.0.0/8 or ::1).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        is_loopback() const;

        /**
         * Indicates whether this address represents a link-local address
         * (169.254.0.0/16 or fe80::/64).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        is_link_local() const;

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
        is_private() const;

        /**
         * Indicates whether this address represents a multicast address
         * (224.0.0.0/4 or ff00::/8).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        is_multicast() const;

        /**
         * Indicates whether this address represents any IANA-reserved
         * address, which could overlap with other flags, such as
         * is_link_local, is_private, is_multicast, etc. See
         * https://en.wikipedia.org/wiki/Reserved_IP_addresses for more
         * information about which other addresses are considered "reserved"
         * that don't already have their own is_* methods here. Note: An
         * IP address's being reserved does not indicate that it cannot be
         * used.
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        is_reserved() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::optional<MulticastScope> const &
        multicast_scope() const;

        [[nodiscard]]
        virtual
        IPAddressVersion
        version() const = 0;

        [[nodiscard]]
        virtual
        ::std::uint8_t
        maximum_prefix_length() const = 0;

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

        [[nodiscard]]
        virtual
        size_t
        data_length() const = 0;

        ::std::string const _representation;
        bool _is_unspecified = false;
        bool _is_loopback = false;
        bool _is_link_local = false;
        bool _is_private = false;
        bool _is_multicast = false;
        bool _is_reserved = false;
        ::std::optional< MulticastScope > _multicast_scope;
    };

    class OddSource_Export IPv4Address : public IPAddress
    {
    public:
        IPv4Address() = delete;

        // conversion constructor
        OddSource_Inline
        explicit
        IPv4Address(
            ::std::string_view const & );

        // conversion constructor
        OddSource_Inline
        explicit
        IPv4Address(
        in_addr const *);

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
        ~IPv4Address() noexcept;

        [[nodiscard]]
        OddSource_Inline
        explicit
        operator in_addr const *() const;

        [[nodiscard]]
        OddSource_Inline
        IPAddressVersion
        version() const final;

        [[nodiscard]]
        OddSource_Inline
        ::std::uint8_t
        maximum_prefix_length() const final;

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

    protected:
        [[nodiscard]]
        OddSource_Inline
        size_t
        data_length() const final;

    private:
        OddSource_Inline
        explicit
        IPv4Address(
            ::std::unique_ptr< in_addr const > && );

        ::std::unique_ptr< in_addr const > _data;
    };

    struct OddSource_Export v6Scope
    {
        ::std::optional< ::std::uint32_t > scope_id = ::std::nullopt;
        ::std::optional< ::std::string > scope_name = ::std::nullopt;
    };

    class OddSource_Export IPv6Address : public IPAddress
    {
    public:
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
        ~IPv6Address() noexcept;

        [[nodiscard]]
        OddSource_Inline
        explicit
        operator in6_addr const *() const;

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
        is_unique_local() const;

        /**
         * Indicates whether this address represents a deprecated site-local
         * address (fec0::/10).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        is_site_local() const;

        /**
         * Indicates whether this address represents an IPv4-mapped address
         * (::ffff:0:0/96, i.e. ::ffff:0.0.0.0 - ::ffff:255.255.255.255).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        is_v4_mapped() const;

        /**
         * Indicates whether this address represents an IPv4-translated address
         * (::ffff:0:0:0/96, i.e. ::ffff:0:0.0.0.0 - ::ffff:0:255.255.255.255).
         *
         * @return true or false.
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        is_v4_translated() const;

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
        is_v4_compatible() const;

        /**
         * Indicates whether this address represents a deprecated 6to4 address
         * (2002::/16, i.e. 2002:: - 2002:ffff:ffff:ffff:ffff:ffff:ffff:ffff).
         *
         * @return true or false;
         */
        [[nodiscard]]
        OddSource_Inline
        bool
        is_6to4() const;

        [[nodiscard]]
        OddSource_Inline
        bool
        has_scope_id() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::string
        without_scope_id() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::optional<::std::uint32_t> const &
        scope_id() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::optional<::std::string> const &
        scope_name() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::optional<::std::string>
        scope_name_or_id() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::optional<::std::string>
        scope_id_or_name() const;

        [[nodiscard]]
        OddSource_Inline
        bool
        is_multicast_flag_enabled(
            MulticastV6Flag const & flag ) const;

        [[nodiscard]]
        OddSource_Inline
        IPAddressVersion
        version() const final;

        [[nodiscard]]
        OddSource_Inline
        ::std::uint8_t
        maximum_prefix_length() const final;

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

    protected:
        [[nodiscard]]
        OddSource_Inline
        size_t
        data_length() const final;

    private:
        OddSource_Inline
        IPv6Address(
            ::std::string_view const & reprWithScope,
            ::std::string_view const & reprWithoutScope );

        OddSource_Inline
        IPv6Address(
            ::std::string && reprWithoutScope,
            ::std::unique_ptr< in6_addr const > && data,
            ::std::optional< v6Scope > && scope );

        ::std::unique_ptr<in6_addr const> _data;
        ::std::optional<v6Scope> const _scope;
        ::std::string const _without_scope;
        bool _is_unique_local = false;
        bool _is_site_local = false;
        bool _is_v4_mapped = false;
        bool _is_v4_compatible = false;
        bool _is_v4_translated = false;
        bool _is_6to4 = false;
        ::std::optional<::std::uint8_t> _multicast_flags;
    };

    OddSource_Export
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        IPAddress const & address );
}

#ifdef IFADDRS4CPP_INLINE_SOURCE
#include "impl/IpAddress.ipp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

#endif /* ODDSOURCE_NETWORK_INTERFACES_IPADDRESS_HPP */
