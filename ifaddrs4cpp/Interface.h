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
#include "winsock_includes.h"
#else /* IS_WINDOWS */
#include <net/if.h>
#include <netinet/in.h>
#include <sys/types.h>
#if __has_include(<net/route.h>)
#include <net/route.h>
#endif /* <net/route.h> */
#if __has_include(<netinet6/in6_var.h>)
#include <netinet6/in6_var.h>
#endif /* <netinet6/in6_var.h> */
#endif /* IS_WINDOWS */

#include <optional>
#include <vector>
#include <unordered_map>

#include "IpAddress.h"
#include "MacAddress.h"

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
    enum InterfaceIPAddressFlag : uint16_t
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

    static ::std::unordered_map<::std::string, InterfaceIPAddressFlag const> const InterfaceIPAddressFlag_Values
    {
        {"Anycast", Anycast},
        {"AutoConfigured", AutoConfigured},
        {"Deprecated", Deprecated},
        {"Detached", Detached},
        {"Duplicated", Duplicated},
        {"Dynamic", Dynamic},
        {"Optimistic", Optimistic},
        {"NoDad", NoDad},
        {"Secured", Secured},
        {"Temporary", Temporary},
        {"Tentative", Tentative},
    };

    static ::std::unordered_map<InterfaceIPAddressFlag, ::std::string const> InterfaceIPAddressFlag_Names;

    enum InterfaceFlag : uint16_t
    {
#ifdef IS_WINDOWS
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
#else /* IS_WINDOWS */
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
#endif /* !IS_WINDOWS */

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

    static ::std::unordered_map<::std::string, InterfaceFlag const> const InterfaceFlag_Values
    {
        {"BroadcastAddressSet", BroadcastAddressSet},
        {"DebugEnabled", DebugEnabled},
        {"IsLoopback", IsLoopback},
        {"IsPointToPoint", IsPointToPoint},
        {"IsRunning", IsRunning},
        {"IsUp", IsUp},
        {"NoARP", NoARP},
        {"PromiscuousModeEnabled", PromiscuousModeEnabled},
        {"ReceiveAllMulticastPackets", ReceiveAllMulticastPackets},
        {"SupportsMulticast", SupportsMulticast},
#ifdef IFF_MASTER
        {"Master", Master},
#endif /* IFF_MASTER */
#ifdef IFF_SIMPLEX
        {"Simplex", Simplex},
#endif /* IFF_SIMPLEX */
#ifdef IFF_SLAVE
        {"IFF_SLAVE", Slave},
#endif /* IFF_SLAVE */
#ifdef IFF_NOTRAILERS
        {"Smart", Smart},
#endif /* IFF_NOTRAILERS */
#ifdef IFF_OACTIVE
        {"TransmissionInProgress", TransmissionInProgress},
#endif /* IFF_OACTIVE */
    };

    static ::std::unordered_map<InterfaceFlag, ::std::string const> InterfaceFlag_Names;

    template<class IPAddressT>
    class InterfaceIPAddress;
    template<class IPAddressT>
    ::std::ostream & operator<<(::std::ostream &, InterfaceIPAddress<IPAddressT> const &);

    template<class IPAddressT>
    class OddSource_Export InterfaceIPAddress
    {
        static_assert(::std::is_base_of_v<IPAddress, IPAddressT>,
                      "the template parameter IPAddressT must derive from IPAddress.");

    public:
        InterfaceIPAddress(IPAddressT const & address, uint16_t flags, uint8_t prefix_length = 0);

        InterfaceIPAddress(
                IPAddressT const & address,
                uint16_t flags,
                uint8_t prefix_length,
                IPAddressT const & broadcast_or_destination,
                bool is_point_to_point = false);

        [[nodiscard]]
        inline operator ::std::string() const; // NOLINT(*-explicit-constructor)

        [[nodiscard]]
        inline explicit operator char const *() const;

        [[nodiscard]]
        inline IPAddressT const & address() const;

        [[nodiscard]]
        inline ::std::optional<uint8_t> const & prefix_length() const;

        [[nodiscard]]
        inline ::std::optional<IPAddressT const> const & broadcast_address() const;

        [[nodiscard]]
        inline ::std::optional<IPAddressT const> const & point_to_point_destination() const;

        [[nodiscard]]
        inline bool is_flag_enabled(InterfaceIPAddressFlag) const;

        [[nodiscard]]
        inline uint16_t flags() const;

        [[nodiscard]]
        inline bool operator==(InterfaceIPAddress<IPAddressT> const & other) const;

        [[nodiscard]]
        inline bool operator!=(InterfaceIPAddress<IPAddressT> const & other) const;
    private:
        friend ::std::ostream &
        operator<<<IPAddressT>(::std::ostream & os, InterfaceIPAddress<IPAddressT> const & address);

        IPAddressT const _address;
        ::std::optional<uint8_t> const _prefix_length;
        ::std::optional<IPAddressT const> const _broadcast;
        ::std::optional<IPAddressT const> const _point_to_point;
        uint16_t const _flags;
    };

    typedef InterfaceIPAddress<IPv4Address> InterfaceIPv4Address;
    typedef InterfaceIPAddress<IPv6Address> InterfaceIPv6Address;

    class Interface;
    ::std::ostream & operator<<(::std::ostream &, Interface const &);

    class OddSource_Export Interface
    {
    public:
        Interface() = delete;

        Interface(
            uint32_t,
            ::std::string_view const &,
#ifdef IS_WINDOWS
            ::std::string_view const &,
#endif /* IS_WINDOWS */
            uint32_t,
            ::std::optional<uint64_t const> const & = ::std::nullopt);

        [[nodiscard]]
        inline uint32_t index() const;

        [[nodiscard]]
        inline ::std::string name() const;

#ifdef IS_WINDOWS
        [[nodiscard]]
        inline ::std::string windows_uuid() const;
#endif /* IS_WINDOWS */

        [[nodiscard]]
        inline bool is_up() const;

        [[nodiscard]]
        inline bool is_loopback() const;

        [[nodiscard]]
        inline bool is_flag_enabled(InterfaceFlag) const;

        [[nodiscard]]
        inline uint32_t flags() const;

        [[nodiscard]]
        inline ::std::optional<uint64_t const> const & mtu() const;

        [[nodiscard]]
        inline bool has_mac_address() const;

        [[nodiscard]]
        inline ::std::optional<MacAddress const> const & mac_address() const;

        [[nodiscard]]
        inline ::std::vector<InterfaceIPAddress<IPv4Address>> const & ipv4_addresses() const;

        [[nodiscard]]
        inline ::std::vector<InterfaceIPAddress<IPv6Address>> const & ipv6_addresses() const;

        static Interface const SAMPLE_INTERFACE;

    private:
        friend ::std::ostream & operator<<(::std::ostream &, Interface const &);
        friend class InterfacesHelper;
        friend class TestInterface;

        static Interface get_sample_interface();

        uint32_t const _index = 0; // DWORD on Windows
        ::std::string const _name;
#ifdef IS_WINDOWS
        ::std::string const _windows_uuid;
#endif /* IS_WINDOWS */
        uint32_t const _flags;
        ::std::optional<uint64_t const> _mtu;
        ::std::optional<MacAddress const> _mac_address;
        ::std::vector<InterfaceIPAddress<IPv4Address>> _ipv4_addresses;
        ::std::vector<InterfaceIPAddress<IPv6Address>> _ipv6_addresses;
    };
}

#include "Interface.hpp"
