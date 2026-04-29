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

#ifndef ODDSOURCE_NETWORK_INTERFACES_INTERFACE_HPP
#define ODDSOURCE_NETWORK_INTERFACES_INTERFACE_HPP

#include "detail/config.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "detail/winsock_includes.h"
#include "IpAddress.hpp"
#include "MacAddress.hpp"

#ifndef ODDSOURCE_IS_WINDOWS
#include <net/if.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <netinet/in.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <sys/types.h>
#if __has_include(<net/route.h>)
#include <net/route.h>
#endif /* <net/route.h> */
#if __has_include(<netinet6/in6_var.h>)
#include <netinet6/in6_var.h>
#endif /* <netinet6/in6_var.h> */
#endif /* !ODDSOURCE_IS_WINDOWS */

#include <optional>
#include <vector>
#include <unordered_map>

// BSD/macOS is the only OS that defines these, and older BSD versions
// are missing the last three. Simply define these on all other
// OSes (Linux, Windows) to match the BSD/macOS constants.
#ifndef IN6_IFF_ANYCAST
#define IN6_IFF_ANYCAST         0x0001  /* anycast address */
#endif
#ifndef IN6_IFF_TENTATIVE
#define IN6_IFF_TENTATIVE       0x0002  /* tentative address */
#endif
#ifndef IN6_IFF_DUPLICATED
#define IN6_IFF_DUPLICATED      0x0004  /* DAD detected duplicate */
#endif
#ifndef IN6_IFF_DETACHED
#define IN6_IFF_DETACHED        0x0008  /* XXX Obsolete. May be detached from the link */
#endif
#ifndef IN6_IFF_DEPRECATED
#define IN6_IFF_DEPRECATED      0x0010  /* deprecated address */
#endif
#ifndef IN6_IFF_NODAD
#define IN6_IFF_NODAD           0x0020 /* do not perform DAD on this address */
#endif
#ifndef IN6_IFF_AUTOCONF
#define IN6_IFF_AUTOCONF        0x0040  /* autoconfigurable address. */
#endif
#ifndef IN6_IFF_TEMPORARY
#define IN6_IFF_TEMPORARY       0x0080  /* temporary (anonymous) address. */
#endif
#ifndef IN6_IFF_DYNAMIC
#define IN6_IFF_DYNAMIC         0x0100  /* assigned by DHCPv6 service */
#endif
#ifndef IN6_IFF_OPTIMISTIC
#define IN6_IFF_OPTIMISTIC      0x0200  /* optimistic DAD, i.e. RFC 4429 */
#endif
#ifndef IN6_IFF_SECURED
#define IN6_IFF_SECURED         0x0400  /* cryptographically generated */
#endif

// some BSD versions
#ifndef IFF_RUNNING
#ifdef IFF_DRV_RUNNING
#define IFF_RUNNING IFF_DRV_RUNNING
#endif
#endif
#ifndef IFF_OACTIVE
#ifdef IFF_DRV_OACTIVE
#define IFF_OACTIVE IFF_DRV_OACTIVE
#endif
#endif

namespace OddSource::Interfaces
{
    enum class OddSource_Export InterfaceIPAddressFlag : ::std::uint16_t
    {
        Anycast = IN6_IFF_ANYCAST,
        AutoConfigured = IN6_IFF_AUTOCONF,
        Deprecated = IN6_IFF_DEPRECATED,
        Detached = IN6_IFF_DETACHED,
        Duplicated = IN6_IFF_DUPLICATED,
        Dynamic = IN6_IFF_DYNAMIC,
        Optimistic = IN6_IFF_OPTIMISTIC,
        NoDad = IN6_IFF_NODAD,
        Secured = IN6_IFF_SECURED,
        Temporary = IN6_IFF_TEMPORARY,
        Tentative = IN6_IFF_TENTATIVE
    };

    OddSource_Export
    ::std::underlying_type_t< InterfaceIPAddressFlag >
    operator&(
        ::std::underlying_type_t< InterfaceIPAddressFlag > lhs,
        InterfaceIPAddressFlag const & rhs );

    OddSource_Export
    ::std::underlying_type_t< InterfaceIPAddressFlag >
    operator|(
        InterfaceIPAddressFlag const & lhs,
        InterfaceIPAddressFlag const & rhs );

    OddSource_Export
    ::std::underlying_type_t< InterfaceIPAddressFlag >
    operator|(
        ::std::underlying_type_t< InterfaceIPAddressFlag > lhs,
        InterfaceIPAddressFlag const & rhs );

    OddSource_Export
    bool
    operator==(
        ::std::underlying_type_t< InterfaceIPAddressFlag > lhs,
        InterfaceIPAddressFlag const & rhs );

    OddSource_Extern OddSource_Export
    ::std::unordered_map< ::std::string, InterfaceIPAddressFlag const > const
    InterfaceIPAddressFlag_Values;

    OddSource_Extern OddSource_Export
    ::std::unordered_map< InterfaceIPAddressFlag, ::std::string const > const
    InterfaceIPAddressFlag_Names;

    OddSource_Export
    ::std::string
    toString(
        InterfaceIPAddressFlag const & flag );

    OddSource_Export
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        InterfaceIPAddressFlag const & flag );

    enum class OddSource_Export InterfaceFlag : ::std::uint16_t
    {
#ifdef ODDSOURCE_IS_WINDOWS
        BroadcastAddressSet = 0x2,
        DebugEnabled = 0x4, // placeholder, not applicable to Windows
        IsLoopback = 0x8,
        IsPointToPoint = 0x10, // placeholder, not applicable to Windows
        IsRunning = 0x40,
        IsUp = 0x1,
        NoARP = 0x80, // placeholder, not applicable to Windows
        PromiscuousModeEnabled = 0x100, // placeholder, not applicable to Windows
        ReceiveAllMulticastPackets = 0x200, // placeholder, not applicable to Windows
        SupportsMulticast = 0x8000, // inverse on Windows, e.g. !IP_ADAPTER_NO_MULTICAST
#else /* ODDSOURCE_IS_WINDOWS */
        BroadcastAddressSet = IFF_BROADCAST,
        DebugEnabled = IFF_DEBUG,
        IsLoopback = IFF_LOOPBACK,
        IsPointToPoint = IFF_POINTOPOINT,
        IsRunning = IFF_RUNNING,
        IsUp = IFF_UP,
        NoARP = IFF_NOARP,
        PromiscuousModeEnabled = IFF_PROMISC,
        ReceiveAllMulticastPackets = IFF_ALLMULTI,
        SupportsMulticast = IFF_MULTICAST,
#endif /* !ODDSOURCE_IS_WINDOWS */

        // *nix-Platform-Specific Flags
#ifdef IFF_MASTER
        Master = IFF_MASTER, // Linux only
#endif /* IFF_MASTER */
#ifdef IFF_SIMPLEX
        Simplex = IFF_SIMPLEX, // *BSD only
#endif /* IFF_SIMPLEX */
#ifdef IFF_SLAVE
        Slave = IFF_SLAVE, // Linux only
#endif /* IFF_SLAVE */
#ifdef IFF_NOTRAILERS
        Smart = IFF_NOTRAILERS, // weird one, macOS only, deprecated constant doesn't match purpose?
#endif /* IFF_NOTRAILERS */
#ifdef IFF_OACTIVE
        TransmissionInProgress = IFF_OACTIVE, // *BSD only
#endif /* IFF_OACTIVE */
    };

    OddSource_Export
    ::std::underlying_type_t< InterfaceFlag >
    operator&(
        ::std::underlying_type_t< InterfaceFlag > lhs,
        InterfaceFlag const & rhs );

    OddSource_Export
    ::std::underlying_type_t< InterfaceFlag >
    operator|(
        InterfaceFlag const & lhs,
        InterfaceFlag const & rhs );

    OddSource_Export
    ::std::underlying_type_t< InterfaceFlag >
    operator|(
        ::std::underlying_type_t< InterfaceFlag > lhs,
        InterfaceFlag const & rhs );

    OddSource_Export
    bool
    operator==(
        ::std::underlying_type_t< InterfaceFlag > lhs,
        InterfaceFlag const & rhs );

    OddSource_Extern OddSource_Export
    ::std::unordered_map< ::std::string, InterfaceFlag const > const
    InterfaceFlag_Values;

    OddSource_Extern OddSource_Export
    ::std::unordered_map< InterfaceFlag, ::std::string const > const
    InterfaceFlag_Names;

    OddSource_Export
    ::std::string
    toString(
        InterfaceFlag const & version );

    OddSource_Export
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        InterfaceFlag const & flag );

    struct Broadcast_t {};
    OddSource_Extern OddSource_Export
    Broadcast_t const Broadcast;

    struct PointToPoint_t {};
    OddSource_Extern OddSource_Export
    PointToPoint_t const PointToPoint;

    template< class IPAddressT >
    class OddSource_Export InterfaceIPAddress
    {
        static_assert( ::std::is_base_of_v< IPAddress, IPAddressT >,
                       "the template parameter IPAddressT must derive from IPAddress.");

    public:
        InterfaceIPAddress(
            IPAddressT const & address,
            ::std::uint16_t flags,
            ::std::uint8_t prefixLength = 0 );

        InterfaceIPAddress(
            IPAddressT const & address,
            ::std::uint16_t flags,
            ::std::uint8_t prefixLength,
            Broadcast_t,
            IPAddressT const & broadcastAddress );

        InterfaceIPAddress(
            IPAddressT const & address,
            ::std::uint16_t flags,
            ::std::uint8_t prefixLength,
            PointToPoint_t,
            IPAddressT const & pointToPointDestination );

        InterfaceIPAddress(
            InterfaceIPAddress const & other );

        InterfaceIPAddress(
            InterfaceIPAddress && other ) noexcept;

        virtual
        ~InterfaceIPAddress() noexcept;

        [[nodiscard]]
        explicit
        operator ::std::string() const;

        [[nodiscard]]
        explicit
        operator char const *() const;

        [[nodiscard]]
        IPAddressT const &
        address() const;

        [[nodiscard]]
        ::std::optional< ::std::uint8_t >
        prefix_length() const;

        [[nodiscard]]
        ::std::optional< IPAddressT const > const &
        broadcast_address() const;

        [[nodiscard]]
        ::std::optional< IPAddressT const > const &
        point_to_point_destination() const;

        [[nodiscard]]
        bool
        is_flag_enabled(
            InterfaceIPAddressFlag flag ) const;

        [[nodiscard]]
        ::std::uint16_t
        flags() const;

        [[nodiscard]]
        bool
        operator==(
            InterfaceIPAddress< IPAddressT > const & other ) const;

        [[nodiscard]]
        bool operator!=(
            InterfaceIPAddress< IPAddressT > const & other ) const;
    private:
        IPAddressT const _address;
        ::std::optional< ::std::uint8_t > const _prefixLength;
        ::std::optional< IPAddressT const > const _broadcast;
        ::std::optional< IPAddressT const > const _pointToPointDestination;
        ::std::uint16_t const _flags;
    };

    template class OddSource_Export InterfaceIPAddress< IPv4Address >;
    template class OddSource_Export InterfaceIPAddress< IPv6Address >;

    typedef InterfaceIPAddress< IPv4Address > InterfaceIPv4Address;
    typedef InterfaceIPAddress< IPv6Address > InterfaceIPv6Address;

    template< class IPAddressT >
    OddSource_Export
    ::std::string
    toString(
        InterfaceIPAddress< IPAddressT > const & address );

    template< class IPAddressT >
    OddSource_Export
    ::std::ostream &
    operator<<(
        ::std::ostream &,
        InterfaceIPAddress< IPAddressT > const & address );

    class OddSource_Export Interface
    {
    public:
        Interface() = delete;

        OddSource_Inline
        Interface(
            ::std::uint32_t index,
            ::std::string_view const & name,
            ::std::string_view const & friendlyName,
            ::std::string_view const & description,
            ::std::uint32_t flags,
            ::std::optional< ::std::uint64_t const > const & mtu = ::std::nullopt);

        OddSource_Inline
        Interface(
            Interface const & other );

        OddSource_Inline
        Interface(
            Interface && other ) noexcept;

        OddSource_Inline
        virtual
        ~Interface() noexcept;

        [[nodiscard]]
        OddSource_Inline
        ::std::uint32_t
        index() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::string
        name() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::string
        friendlyName() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::string
        description() const;

        [[nodiscard]]
        OddSource_Inline
        bool
        is_up() const;

        [[nodiscard]]
        OddSource_Inline
        bool
        is_loopback() const;

        [[nodiscard]]
        OddSource_Inline
        bool
        is_flag_enabled(
            InterfaceFlag const & flag ) const;

        [[nodiscard]]
        OddSource_Inline
        ::std::uint32_t
        flags() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::optional< ::std::uint64_t >
        mtu() const;

        [[nodiscard]]
        OddSource_Inline
        bool
        has_mac_address() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::optional< MacAddress const > const &
        mac_address() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::vector< InterfaceIPv4Address > const &
        ipv4_addresses() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::vector< InterfaceIPv6Address > const &
        ipv6_addresses() const;

        static
        OddSource_Inline
        Interface
        getSampleInterface();

    private:
        friend
        OddSource_Export
        ::std::ostream &
        operator<<(
            ::std::ostream & os,
            Interface const & rInterface );

        friend class InterfaceBrowser;

        friend class TestInterface;

        ::std::uint32_t _index; // DWORD on Windows
        ::std::string _name;
        ::std::string _friendlyName;
        ::std::string _description;
        ::std::uint32_t _flags;
        ::std::optional< ::std::uint64_t const > _mtu;
        ::std::optional< MacAddress const > _macAddress;
        ::std::vector< InterfaceIPv4Address > _ipv4Addresses;
        ::std::vector< InterfaceIPv6Address > _ipv6Addresses;
    };

    OddSource_Export
    ::std::string
    toString(
        Interface const & rInterface );
}

#include "detail/Interface.hpp"

#ifdef IFADDRS4CPP_INLINE_SOURCE
#include "impl/Interface.ipp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

#endif /* ODDSOURCE_NETWORK_INTERFACES_INTERFACE_HPP */
