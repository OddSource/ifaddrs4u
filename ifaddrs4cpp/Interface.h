#pragma once

#include "config.h"

#ifdef IS_WINDOWS
#else /* IS_WINDOWS */
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <sys/socket.h>
#include <net/route.h>
#ifdef IS_MACOS
#include <netinet6/in6_var.h>
#endif /* IS_MACOS */
#ifdef IS_BSD
#include <net/route.h>
#endif /* IS_BSD */
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
        BroadcastAddressSet = IFF_BROADCAST, // no Windows
        DebugEnabled = IFF_DEBUG, // no Windows
        IsLoopback = IFF_LOOPBACK,
        IsPointToPoint = IFF_POINTOPOINT, // no Windows
        IsRunning = IFF_RUNNING,
        IsUp = IFF_UP,
        NoARP = IFF_NOARP, // no Windows
        NoTrailers = IFF_NOTRAILERS, // aka "SMART" ... no Windows
        PromiscuousModeEnabled = IFF_PROMISC, // no Windows
        ReceiveAllMulticastPackets = IFF_ALLMULTI,
        SupportsMulticast = IFF_MULTICAST, // inverse on Windows

        // *nix-Platform-Specific Flags
#ifdef IFF_OACTIVE
        TransmissionInProgress = IFF_OACTIVE, // *BSD only
#endif /* IFF_OACTIVE */
#ifdef IFF_SIMPLEX
        Simplex = IFF_SIMPLEX, // *BSD only
#endif /* IFF_SIMPLEX */
#ifdef IFF_MASTER
        Master = IFF_MASTER, // Linux only
#endif /* IFF_MASTER */
#ifdef IFF_SLAVE
        Slave = IFF_SLAVE, // Linux only
#endif /* IFF_SLAVE */
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
        {"NoTrailers", NoTrailers},
        {"PromiscuousModeEnabled", PromiscuousModeEnabled},
        {"ReceiveAllMulticastPackets", ReceiveAllMulticastPackets},
        {"SupportsMulticast", SupportsMulticast},
#ifdef IFF_OACTIVE
        {"TransmissionInProgress", TransmissionInProgress},
#endif /* IFF_OACTIVE */
#ifdef IFF_SIMPLEX
        {"Simplex", Simplex},
#endif /* IFF_SIMPLEX */
#ifdef IFF_MASTER
        {"Master", Master},
#endif /* IFF_MASTER */
#ifdef IFF_SLAVE
        {"IFF_SLAVE", IFF_SLAVE},
#endif /* IFF_SLAVE */
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
        inline operator char const *() const; // NOLINT(*-explicit-constructor)

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
        inline ::std::optional<uint64_t const> const & mtu() const;

        [[nodiscard]]
        inline bool has_mac_address() const;

        [[nodiscard]]
        inline ::std::optional<MacAddress const> const & mac_address() const;

        [[nodiscard]]
        inline ::std::vector<InterfaceIPAddress<IPv4Address> const> const & ipv4_addresses() const;

        [[nodiscard]]
        inline ::std::vector<InterfaceIPAddress<IPv6Address> const> const & ipv6_addresses() const;

    private:
        friend ::std::ostream & operator<<(::std::ostream &, Interface const &);
        friend class InterfacesHelper;
        friend class TestInterface;

        uint32_t const _index = 0; // DWORD on Windows
        ::std::string const _name;
#ifdef IS_WINDOWS
        ::std::string const _windows_uuid;
#endif /* IS_WINDOWS */
        uint32_t const _flags;
        ::std::optional<uint64_t const> _mtu;
        ::std::optional<MacAddress const> _mac_address;
        ::std::vector<InterfaceIPAddress<IPv4Address> const> _ipv4_addresses;
        ::std::vector<InterfaceIPAddress<IPv6Address> const> _ipv6_addresses;
    };
}

#include "Interface.hpp"
