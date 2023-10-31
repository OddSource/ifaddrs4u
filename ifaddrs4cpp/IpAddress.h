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

#include "config.h"

#ifdef IS_WINDOWS
#include <winsock2.h>
#else /* IS_WINDOWS */
#include <netinet/in.h>
#endif /* IS_WINDOWS */

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

namespace OddSource::Interfaces
{
#ifdef IS_WINDOWS
    typedef USHORT AddressFamily;
#else /* IS_WINDOWS */
    typedef sa_family_t AddressFamily;
#endif /* IS_WINDOWS */

    class OddSource_Export InvalidIPAddress : public ::std::invalid_argument
    {
    public:
        explicit InvalidIPAddress(::std::string_view const & what)
            : ::std::invalid_argument(::std::string(what)) {}
        InvalidIPAddress(InvalidIPAddress const & other) noexcept = default;
    };

    enum OddSource_Export IPAddressVersion : uint8_t
    {
        IPv4 = 4,
        IPv6 = 6
    };

    enum OddSource_Export MulticastScope : uint16_t
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

    enum OddSource_Export MulticastV6Flag : uint8_t
    {
        DynamicallyAssigned = 0b0001,
        PrefixBased = 0b0010,
        RendezvousEmbedded = 0b0100,
        ReservedFlag = 0b1000 // unused, here for clarity and unit testing
    };

    class OddSource_Export IPAddress
    {
    public:
        virtual ~IPAddress();

        /**
         * Converts the IP address to a C++ string.
         *
         * @return a string.
         */
        [[nodiscard]]
        virtual inline operator ::std::string() const; // NOLINT(*-explicit-constructor)

        /**
         * Converts the IP address to a C string.
         *
         * @return a string.
         */
        [[nodiscard]]
        virtual inline operator char const *() const; // NOLINT(*-explicit-constructor)

        /**
         * Indicates whether this address represents the unspecified address
         * (0.0.0.0 or ::0).
         *
         * @return true or false.
         */
        [[nodiscard]]
        inline bool is_unspecified() const;

        /**
         * Indicates whether this address represents the loopback address
         * (127.0.0.0/8 or ::1).
         *
         * @return true or false.
         */
        [[nodiscard]]
        inline bool is_loopback() const;

        /**
         * Indicates whether this address represents a link-local address
         * (169.254.0.0/16 or fe80::/64).
         *
         * @return true or false.
         */
        [[nodiscard]]
        inline bool is_link_local() const;

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
        inline bool is_private() const;

        /**
         * Indicates whether this address represents a multicast address
         * (224.0.0.0/4 or ff00::/8).
         *
         * @return true or false.
         */
        [[nodiscard]]
        inline bool is_multicast() const;

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
        inline bool is_reserved() const;

        [[nodiscard]]
        inline ::std::optional<MulticastScope> const & multicast_scope() const;

        [[nodiscard]]
        virtual inline IPAddressVersion version() const = 0;

        [[nodiscard]]
        virtual inline uint8_t maximum_prefix_length() const = 0;

    protected:
        IPAddress(::std::string_view const &); // NOLINT(*-explicit-constructor)

        // copy constructor
        IPAddress(IPAddress const &);

        // move constructor
        IPAddress(IPAddress &&) noexcept;

        [[nodiscard]]
        inline virtual size_t data_length() const = 0;

        ::std::string const _representation;
        bool _is_unspecified = false;
        bool _is_loopback = false;
        bool _is_link_local = false;
        bool _is_private = false;
        bool _is_multicast = false;
        bool _is_reserved = false;
        ::std::optional<MulticastScope> _multicast_scope;
    };

    struct IPv4TempDataHolder;

    class OddSource_Export IPv4Address : public IPAddress
    {
    public:
        IPv4Address() = delete;

        // copy constructor
        IPv4Address(IPv4Address const &);

        // move constructor
        IPv4Address(IPv4Address &&) noexcept = default;

        // conversion constructor
        IPv4Address(::std::string_view const &); // NOLINT(*-explicit-constructor)

        // conversion constructor
        IPv4Address(in_addr const *); // NOLINT(*-explicit-constructor)

        ~IPv4Address() override = default;

        [[nodiscard]]
        inline operator in_addr const *() const; // NOLINT(*-explicit-constructor)

        [[nodiscard]]
        inline IPAddressVersion version() const final;

        [[nodiscard]]
        inline uint8_t maximum_prefix_length() const final;

        [[nodiscard]]
        inline bool operator==(IPv4Address const &) const;

        [[nodiscard]]
        inline bool operator!=(IPv4Address const &) const;

    protected:
        [[nodiscard]]
        inline size_t data_length() const final;

    private:
        explicit IPv4Address(IPv4TempDataHolder const &);

        ::std::unique_ptr<in_addr const> _data;
    };

    struct OddSource_Export v6Scope
    {
        ::std::optional<uint32_t> scope_id = ::std::nullopt;
        ::std::optional<::std::string> scope_name = ::std::nullopt;
    };

    struct IPv6TempDataHolder;

    class OddSource_Export IPv6Address : public IPAddress
    {
    public:
        IPv6Address() = delete;

        // copy constructor
        IPv6Address(IPv6Address const &);

        // move constructor
        IPv6Address(IPv6Address &&) noexcept = default;

        // conversion constructor
        IPv6Address(::std::string_view const &); // NOLINT(*-explicit-constructor)

        // conversion constructor
        IPv6Address(in6_addr const *); // NOLINT(*-explicit-constructor)

        IPv6Address(in6_addr const *, uint32_t);

        IPv6Address(in6_addr const *, ::std::string_view const &);

        IPv6Address(in6_addr const *, v6Scope const & scope);

        ~IPv6Address() override = default;

        [[nodiscard]]
        inline operator in6_addr const *() const; // NOLINT(*-explicit-constructor)

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
        IPv6Address normalize() const;

        /**
         * Indicates whether this address represents a unique-local address,
         * (fc00::/7).
         *
         * @return true or false.
         */
        [[nodiscard]]
        inline bool is_unique_local() const;

        /**
         * Indicates whether this address represents a deprecated site-local
         * address (fec0::/10).
         *
         * @return true or false.
         */
        [[nodiscard]]
        inline bool is_site_local() const;

        /**
         * Indicates whether this address represents an IPv4-mapped address
         * (::ffff:0:0/96, i.e. ::ffff:0.0.0.0 - ::ffff:255.255.255.255).
         *
         * @return true or false.
         */
        [[nodiscard]]
        inline bool is_v4_mapped() const;

        /**
         * Indicates whether this address represents an IPv4-translated address
         * (::ffff:0:0:0/96, i.e. ::ffff:0:0.0.0.0 - ::ffff:0:255.255.255.255).
         *
         * @return true or false.
         */
        [[nodiscard]]
        inline bool is_v4_translated() const;

        /**
         * Indicates whether this address represents a deprecated IPv4-compatible
         * address (::0/96 from ::2, i.e. ::0.0.0.2 - ::255.255.255.255). These
         * are deprecated for numerous reasons, one of which is that addresses
         * 0.0.0.0-0.0.0.1 cannot be represented.
         *
         * @return true or false.
         */
        [[nodiscard]]
        inline bool is_v4_compatible() const;

        /**
         * Indicates whether this address represents a deprecated 6to4 address
         * (2002::/16, i.e. 2002:: - 2002:ffff:ffff:ffff:ffff:ffff:ffff:ffff).
         *
         * @return true or false;
         */
        [[nodiscard]]
        inline bool is_6to4() const;

        [[nodiscard]]
        inline bool has_scope_id() const;

        [[nodiscard]]
        inline ::std::string without_scope_id() const;

        [[nodiscard]]
        inline ::std::optional<uint32_t> const & scope_id() const;

        [[nodiscard]]
        inline ::std::optional<::std::string> const & scope_name() const;

        [[nodiscard]]
        inline ::std::optional<::std::string> scope_name_or_id() const;

        [[nodiscard]]
        inline ::std::optional<::std::string> scope_id_or_name() const;

        [[nodiscard]]
        inline bool is_multicast_flag_enabled(MulticastV6Flag flag) const;

        [[nodiscard]]
        inline IPAddressVersion version() const final;

        [[nodiscard]]
        inline uint8_t maximum_prefix_length() const final;

        [[nodiscard]]
        inline bool operator==(IPv6Address const &) const;

        [[nodiscard]]
        inline bool operator!=(IPv6Address const &) const;

    protected:
        [[nodiscard]]
        inline size_t data_length() const final;

    private:
        IPv6Address(::std::string const &, IPv6TempDataHolder const &);

        [[nodiscard]]
        static ::std::string_view strip_scope(::std::string_view const &);

        [[nodiscard]]
        static ::std::optional<v6Scope> extract_scope(::std::string_view const &);

        [[nodiscard]]
        static ::std::string add_scope(::std::string const &, ::std::optional<v6Scope> const &);

        ::std::unique_ptr<in6_addr const> _data;
        ::std::optional<v6Scope> const _scope;
        ::std::string const _without_scope;
        bool _is_unique_local = false;
        bool _is_site_local = false;
        bool _is_v4_mapped = false;
        bool _is_v4_compatible = false;
        bool _is_v4_translated = false;
        bool _is_6to4 = false;
        ::std::optional<uint8_t> _multicast_flags;
    };

    inline ::std::ostream & operator<<(::std::ostream &, IPAddress const &);
}

#include "IpAddress.hpp"
