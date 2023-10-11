#include "config.h"

#ifdef IS_WINDOWS
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#else /* IS_WINDOWS */

#include <cerrno>
#include <cstring>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>

#if __has_include(<linux/ipv6.h>)
#include <linux/ipv6.h>
#define # HAS_LINUX_IPV6
#endif /* <linux/ipv6.h> */

#if __has_include(<net/if_dl.h>)
#include <net/if_dl.h>
#elif __has_include(<linux/if_packet.h>)
#include <linux/if_packet.h>
#else
#error "Either net/if_dl.h or linux/if_packet.h must be present on the system"
#endif /* !<net/if_dl.h> && !<linux/if_packet.h> */
#endif /* !IS_WINDOWS */
#include <cassert>
#include <functional>
#include <shared_mutex>
#include <string>

#include "Interface.h"
#include "Interfaces.h"
#include "IpAddress.h"
#include "MacAddress.h"

#ifdef IS_WINDOWS
#define WORKING_BUFFER_SIZE 15000
#define MAX_TRIES 3

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))
#else /* IS_WINDOWS */
#ifdef AF_LINK
#define AF_MAC_ADDRESS AF_LINK
#else /* AF_LINK */
#define AF_MAC_ADDRESS AF_PACKET
#endif /* AF_LINK */
#endif /* IS_WINDOWS */

namespace OddSource::Interfaces
{
    class InterfacesHelper
    {
    public:
        InterfacesHelper()
#ifdef IS_WINDOWS
            : _pAddresses(nullptr)
#else
            : _ifaddrs(nullptr)
#endif
        {
#ifdef IS_WINDOWS
            DWORD dwRetVal = 0;
            ULONG iterations = 0;
            ULONG const flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER;

            // Allocate a 15 KB buffer to start with.
            ULONG outBufLen = WORKING_BUFFER_SIZE;

            do
            {
                this->_pAddresses = reinterpret_cast <IP_ADAPTER_ADDRESSES *>(MALLOC(outBufLen));
                if (this->_pAddresses == nullptr)
                {
                    throw InterfaceBrowserSystemError("Memory allocation failed for IP_ADAPTER_ADDRESSES struct");
                }

                dwRetVal = ::GetAdaptersAddresses(
                        AF_UNSPEC, flags, nullptr, this->_pAddresses, &outBufLen);

                if (dwRetVal == ERROR_BUFFER_OVERFLOW)
                {
                    FREE(this->_pAddresses);
                    this->_pAddresses = nullptr;
                }
                else
                {
                    break;
                }

                iterations++;
            } while (dwRetVal == ERROR_BUFFER_OVERFLOW && iterations < MAX_TRIES);

            if (dwRetVal != NO_ERROR)
            {
                FREE(this->_pAddresses);
                throw InterfaceBrowserSystemError(
                        ::std::string("Call to GetAdaptersAddresses failed with error code: ") +
                        ::std::to_string(dwRetVal));
            }
#else /* IS_WINDOWS */
            if (::getifaddrs(&this->_ifaddrs) == -1)
            {
                throw InterfaceBrowserSystemError(::strerror(errno));
            }
#endif /* IS_WINDOWS */
        }

        ~InterfacesHelper()
        {
#ifdef IS_WINDOWS
            FREE(this->_pAddresses);
#else /* IS_WINDOWS */
            ::freeifaddrs(this->_ifaddrs);
#endif /* IS_WINDOWS */
        }

        bool for_each_interface(::std::function<bool(Interface &)> & do_this) const
        {
#ifdef IS_WINDOWS
            PIP_ADAPTER_ADDRESSES ifa = this->_pAddresses;
            while (ifa)
            {
                uint16_t flags(0);
                if (ifa->IfType == IF_TYPE_SOFTWARE_LOOPBACK)
                {
                    flags |= IsLoopback;
                }
                if (ifa->OperStatus == IfOperStatusUp)
                {
                    flags |= IsUp;
                    flags |= IsRunning;
                }
                // this might lead os to BroadcastAddressSet, have to determine later
                /*PIP_ADAPTER_PREFIX pre = ifa->FirstPrefix;
                while (pre)
                {
                    pre = pre->Next;
                }*/
                if (ifa->Flags & IP_ADAPTER_NO_MULTICAST != IP_ADAPTER_NO_MULTICAST)
                {
                    flags |= SupportsMulticast;
                }
                Interface interface(
                    ifa->IfIndex,
                    ::std::wstring_convert<::std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(ifa->FriendlyName),
                    ifa->AdapterName.substr(1, uuid.length() - 2), // strip {}
                    flags)

                if (ifa->PhysicalAddress)
                {
                    interface._mac_address.emplace(ifa->PhysicalAddress, ifa->PhysicalAddressLength);
                }

                PIP_ADAPTER_UNICAST_ADDRESS unicast = ifa->FirstUnicastAddress;
                while (unicast)
                {
                    LPSOCKADDR sa = unicast->Address.lpSockaddr;
                    if (sa->sa_family == AF_INET)
                    {
                        InterfacesHelper::add_ipv4_address(
                                sa, ifa->FirstPrefix, interface, unicast->OnLinkPrefixLength);
                    }
                    else if (sa->sa_family == AF_INET6)
                    {
                        uint16_t addr_flags(0);
                        if (unicast->SuffixOrigin == IpSuffixOriginRandom)
                        {
                            addr_flags |= Temporary;
                        }
                        InterfacesHelper::add_ipv6_address(
                                sa, interface, unicast->OnLinkPrefixLength, addr_flags);
                    }
                    unicast = unicast->Next;
                }

                PIP_ADAPTER_ANYCAST_ADDRESS anycast = ifa->FirstAnycastAddress;
                while (anycast)
                {
                    LPSOCKADDR sa = anycast->Address.lpSockaddr;
                    if (sa->sa_family == AF_INET)
                    {
                        // Extremely unlikely, as IPv4 doesn't natively support Anycast
                        // (works only with BGP), but it's Windows, so there's no telling.
                        InterfacesHelper::add_ipv4_address(sa, ifa->FirstPrefix, interface);
                    }
                    else if (sa->sa_family == AF_INET6)
                    {
                        uint16_t addr_flags(Anycast);
                        InterfacesHelper::add_ipv6_address(sa, interface, 0, addr_flags);
                    }
                    anycast = anycast->Next;
                }

                if (!do_this(interface))
                {
                    return false;
                }

                ifa = ifa->Next;
            }

            return true;
#else /* IS_WINDOWS */
            // Unfortunately, all of a given interface's ifaddrs entries are not
            // clustered together in the linked list, and instead may be scattered
            // throughout the linked list among other interfaces' ifaddrs entries.
            // As a result, we cannot invoke the callback as we are processing
            // ifaddrs, but instead must fully process ifaddrs, grouping all of the
            // data for each interface as we do, and then we can process that
            // grouping into Interface objects for invoking the callback.
            ifaddrs *ifa = this->_ifaddrs;
            ::std::unordered_map<::std::string, Interface> interfaces;
            ::std::unordered_map<uint32_t, ::std::string> indexes_to_names;
            while (ifa)
            {
                ::std::string name(ifa->ifa_name);
                auto found = interfaces.find(name);
                if (found == interfaces.end())
                {
                    uint32_t index(::if_nametoindex(ifa->ifa_name));
                    // Do something if index == 0 unexpectedly? That means the OS is misbehaving.
                    interfaces.emplace(name, Interface(index, name, ifa->ifa_flags));
                    indexes_to_names.emplace(index, name);
                    found = interfaces.find(name);
                }

                Interface & interface = found->second;
                if (ifa->ifa_flags != interface._flags)
                {
                    // TODO what should we do in this case? Does this mean the OS is misbehaving?
                    ::std::cerr << "Flags " << ::std::to_string(ifa->ifa_flags)
                                << " for next item in interface " << name
                                << " does not match first flags " << ::std::to_string(interface._flags)
                                << ::std::endl;
                }

                if (ifa->ifa_addr)
                {
                    AddressFamily family(ifa->ifa_addr->sa_family);
                    if (family == AF_MAC_ADDRESS)
                    {
                        InterfacesHelper::set_mac_address(ifa, interface);
                    }
                    else if (family == AF_INET)
                    {
                        InterfacesHelper::add_ipv4_address(ifa, interface);
                    }
                    else if (family == AF_INET6)
                    {
                        InterfacesHelper::add_ipv6_address(ifa, interface, indexes_to_names);
                    }
                    else
                    {
                        ::std::cerr << "Unrecognized address family " << ::std::to_string(family)
                                    << " on interface " << name << ::std::endl;
                    }
                }

                ifa = ifa->ifa_next;
            }

            for (auto & [name, interface] : interfaces) // NOLINT(*-use-anyofallof)
            {
                if (!do_this(interface))
                {
                    return false;
                }
            }

            return true;
#endif /* !IS_WINDOWS */
        }

    private:
#ifdef IS_WINDOWS
        static void add_ipv4_address(
            LPSOCKADDR sa, PIP_ADAPTER_PREFIX pre, Interface & interface,
            uint8_t prefix_length = 0)
        {
            auto addr = reinterpret_cast<sockaddr_in *>(sa);
            IPv4Address const address(&addr->sin_addr);

            sockaddr_in * broadcast;
            while (pre)
            {
                LPSOCKADDR candidate = pre->Address.lpSockaddr;
                if (candidate->sa_family == AF_INET)
                {
                    auto cand = reinterpret_cast<sockaddr_in *>(candidate);
                    auto cand_bytes = reinterpret_cast<uint8_t * bytes>(cand.sin_addr.s_addr);
                    uint8_t i, first_byte_with_bcast(0);
                    for (i = 3; i >= 0; i--)
                    {
                        if (cand_bytes[i] == 0xff)
                        {
                            first_byte_with_bcast = i;
                        }
                    }
                    if (first_byte_with_bcast < 1)
                    {
                        continue;
                    }
                    auto addr_bytes = reinterpret_cast<uint8_t * bytes>(addr.sin_addr.s_addr);
                    bool do_continue(false);
                    for (i = 0; i < first_byte_with_bcast; i++)
                    {
                        if (cand_bytes[i] != addr_bytes[i])
                        {
                            do_continue = true;
                            break;
                        }
                    }
                    if (do_continue)
                    {
                        continue;
                    }
                    broadcast = cand;
                    break;
                }
                pre = pre->Next;
            }

            static uint32_t const flags(0);
            if (broadcast)
            {
                interface._ipv4_addresses.emplace_back(
                    address, flags, prefix_length,
                    IPv4Address(&broadcast->sin_addr));
            }
            else
            {
                interface._ipv4_addresses.emplace_back(address, flags, prefix_length);
            }
        }

        static void add_ipv6_address(
            LPSOCKADDR sa, Interface & interface,
            uint8_t prefix_length = 0, uint16_t flags = 0)
        {
            auto addr = reinterpret_cast<sockaddr_in6 *>(sa);
            ::std::optional<::std::string const> scope_id;
            if (addr->sin6_scope_id)
            {
                // On Windows, the scope ID is displayed as the interface number that
                // it is, not as the interface name, probably because the interface name
                // is a gazillion characters long.
                scope_id.emplace(::std::to_string(addr->sin6_scope_id));
            }
            IPv6Address const address(&addr->sin6_addr, scope_id);

            interface._ipv6_addresses.emplace_back(address, flags, prefix_length);
        }

        PIP_ADAPTER_ADDRESSES _pAddresses;
#else /* IS_WINDOWS */
        static void set_mac_address(ifaddrs *ifa, Interface & interface)
        {
#ifdef AF_LINK
            assert(ifa->ifa_addr->sa_family == AF_LINK);
            auto addr = reinterpret_cast<sockaddr_dl *>(ifa->ifa_addr);
            // LLADDR returns a signed char, not unsigned, but the data itself is
            // actually unsigned. (This is because addr->sdl_data contains two sets
            // of data in a signed char, and LLADDR merely extracts the one we need.)
            // So we have to cast to unsigned in order to make use of the data.
            auto data = reinterpret_cast<uint8_t const *>(LLADDR(addr));
            auto data_length = addr->sdl_alen; // should always be 6, but you never know
#else /* AF_LINK */
            assert(ifa->ifa_addr->sa_family == AF_PACKET);
            auto addr = reinterpret_cast<sockaddr_ll *>(ifa->ifa_addr);
            if (addr->sll_hatype != ARPHRD_ETHER)
            {
                return;
            }
            auto data = addr->sll_addr[i];
            auto data_length = addr->sll_halen; // should always be 6, but you never know
#endif /* !AF_LINK */
            interface._mac_address.emplace(data, data_length);
        }

        static void add_ipv4_address(ifaddrs *ifa, Interface & interface)
        {
            auto addr = reinterpret_cast<sockaddr_in *>(ifa->ifa_addr);
            IPv4Address const address(&addr->sin_addr);

            uint8_t prefix_length(0);
            if (ifa->ifa_netmask)
            {
                auto netmask = reinterpret_cast<sockaddr_in *>(ifa->ifa_netmask);
                auto netmask_int = static_cast<uint32_t>(netmask->sin_addr.s_addr);
                while (netmask_int > 0)
                {
                    netmask_int = netmask_int >> 1;
                    prefix_length++;
                }
            }

            static uint32_t const flags(0);
            if (interface.is_flag_enabled(BroadcastAddressSet) && ifa->ifa_broadaddr)
            {
                auto broadcast = reinterpret_cast<sockaddr_in *>(ifa->ifa_broadaddr);
                interface._ipv4_addresses.emplace_back(
                    address, flags, prefix_length,
                    IPv4Address(&broadcast->sin_addr));
            }
            else if(interface.is_flag_enabled(IsPointToPoint) && ifa->ifa_dstaddr)
            {
                auto point_to_point = reinterpret_cast<sockaddr_in *>(ifa->ifa_dstaddr);
                interface._ipv4_addresses.emplace_back(
                    address, flags, prefix_length,
                    IPv4Address(&point_to_point->sin_addr), true);
            }
            else
            {
                interface._ipv4_addresses.emplace_back(address, flags, prefix_length);
            }
        }

        static void add_ipv6_address(
            ifaddrs *ifa,
            Interface & interface,
            ::std::unordered_map<uint32_t, ::std::string> & indexes_to_names)
        {
            auto addr = reinterpret_cast<sockaddr_in6 *>(ifa->ifa_addr);
            ::std::optional<::std::string const> scope_id;
            if (addr->sin6_scope_id)
            {
                auto found_name = indexes_to_names.find(addr->sin6_scope_id);
                if (found_name != indexes_to_names.end())
                {
                    scope_id.emplace(found_name->second);
                }
                else
                {
                    char buffer[IF_NAMESIZE];
                    if (::if_indextoname(addr->sin6_scope_id, buffer))
                    {
                        scope_id.emplace(buffer);
                        indexes_to_names.emplace(addr->sin6_scope_id, *scope_id);
                    }
                }
            }
            IPv6Address const address(&addr->sin6_addr, scope_id);

            uint8_t prefix_length(0);
            if (ifa->ifa_netmask)
            {
                auto netmask = reinterpret_cast<sockaddr_in6 *>(ifa->ifa_netmask);
                auto netmask_bytes = static_cast<uint8_t *>(netmask->sin6_addr.s6_addr);
                uint8_t i(0), n;
                while (i < 16)
                {
                    n = netmask_bytes[i];
                    while (n > 0)
                    {
                        if (n & 1)
                        {
                            prefix_length++;
                        }
                        n = n/2;
                    }
                    i++;
                }
            }

            uint32_t flags(0);
#ifdef SIOCGIFAFLAG_IN6
            in6_ifreq ifr6 {};
            ::strncpy(ifr6.ifr_name, ifa->ifa_name, IFNAMSIZ);
            ifr6.ifr_addr = *addr;
            int sock6(::socket(AF_INET6, SOCK_DGRAM, 0));
            if (sock6 < 0)
            {
                // TODO print error?
            }
            else if (::ioctl(sock6, SIOCGIFAFLAG_IN6, &ifr6) >= 0)
            {
                // IPv6 addresses can have their own flags in addition to the interface's flags,
                // but only some OSes give you access to them
                flags = ifr6.ifr_ifru.ifru_flags6;
            }
#endif /* SIOCGIFAFLAG_IN6 */

            interface._ipv6_addresses.emplace_back(address, flags, prefix_length);
        }

        struct ifaddrs * _ifaddrs;
#endif /* !IS_WINDOWS */
    };
}

bool
OddSource::Interfaces::InterfaceBrowser::
for_each_interface(::std::function<bool(Interface const &)> & do_this)
{
    ::std::shared_lock<::std::shared_mutex> shared(this->_storage_mutex);
    if (!this->_storage_filled)
    {
        shared.unlock();
        ::std::unique_lock<::std::shared_mutex> unique(this->_storage_mutex);
        if (!this->_storage_filled)
        {
            this->populate_and_maybe_more_unsafe(&do_this);
            return true;
        }
        // This only happens if the storage got filled between the shared.unlock()
        // and the unique.lock() and the if block evaluated false.
        unique.unlock();
        shared.lock();
    }

    return ::std::all_of(this->_interface_vector.begin(), this->_interface_vector.end(), do_this);
}

::std::vector<OddSource::Interfaces::Interface const> const &
OddSource::Interfaces::InterfaceBrowser::
get_interfaces()
{
    this->populate_interface_storage();
    ::std::shared_lock<::std::shared_mutex> shared(this->_storage_mutex);
    return this->_interface_vector;

}

OddSource::Interfaces::Interface const &
OddSource::Interfaces::InterfaceBrowser::
get_interface(uint32_t index)
{
    this->populate_interface_storage();
    ::std::shared_lock<::std::shared_mutex> shared(this->_storage_mutex);
    auto found = this->_index_map.find(index);
    if (found == this->_index_map.end())
    {
        throw ::std::invalid_argument(::std::string("No interface found with index: ") + ::std::to_string(index));
    }
    return *found->second;
}

OddSource::Interfaces::Interface const &
OddSource::Interfaces::InterfaceBrowser::
get_interface(::std::string_view const & name)
{
    this->populate_interface_storage();
    ::std::shared_lock<::std::shared_mutex> shared(this->_storage_mutex);
    auto found = this->_name_map.find(::std::string(name));
    if (found == this->_name_map.end())
    {
        throw ::std::invalid_argument(::std::string("No interface found with name: ") + ::std::string(name));
    }
    return *found->second;
}

void
OddSource::Interfaces::InterfaceBrowser::
populate_interface_storage()
{
    ::std::shared_lock<::std::shared_mutex> shared(this->_storage_mutex);
    if (!this->_storage_filled)
    {
        shared.unlock();
        ::std::unique_lock<::std::shared_mutex> unique(this->_storage_mutex);
        if (!this->_storage_filled)
        {
            this->populate_and_maybe_more_unsafe(nullptr);
        }
    }
}

bool
OddSource::Interfaces::InterfaceBrowser::
populate_and_maybe_more_unsafe(::std::function<bool(Interface const &)> * do_this)
{
    bool return_internal(true);
    ::std::vector<Interface const> temp_vector;
    ::std::unordered_map<uint32_t, ::std::shared_ptr<Interface const>> temp_index_map;
    ::std::unordered_map<::std::string, ::std::shared_ptr<Interface const>> temp_name_map;
    ::std::function<bool(Interface &)> do_this_internal = [&](auto interface)
    {
        if (return_internal && do_this)
        {
            return_internal = (*do_this)(interface);
        }
        auto ptr = ::std::make_shared<Interface const>(interface);
        temp_index_map.insert(std::make_pair(interface.index(), ptr));
        temp_name_map.insert(std::make_pair(interface.name(), ptr));
#if IS_WINDOWS
        temp_name_map.insert(std::make_pair(interface.windows_uuid(), ptr));
        temp_name_map.insert(std::make_pair(::std::string("{") + interface.windows_uuid() + ::std::string("}"), ptr));
#endif /* IS_WINDOWS */
        temp_vector.push_back(::std::move(interface));

        return true;
    };
    InterfacesHelper().for_each_interface(do_this_internal);
    this->_interface_vector = ::std::move(temp_vector);
    this->_index_map = ::std::move(temp_index_map);
    this->_name_map = ::std::move(temp_name_map);
    return return_internal;
}

OddSource::Interfaces::WinSockStartupCleanupHelper::
WinSockStartupCleanupHelper()
{
#ifdef IS_WINDOWS
    WORD version_requested = MAKEWORD(2, 2);
    WSADATA data;
    int error = WSAStartup(version_requested, &data);
    if (error != 0)
    {
        throw ::std::system_error(
            ::std::string("Could not initialize WinSock subsystem due to error code: ") +
            ::std::to_string(error) + ". For the meaning of this, see the documentation: " +
            "https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup#return-value");
    }
#endif /* IS_WINDOWS */
}

OddSource::Interfaces::WinSockStartupCleanupHelper::
~WinSockStartupCleanupHelper()
{
#ifdef IS_WINDOWS
    WSACleanup();
#endif /* IS_WINDOWS */
}

OddSource::Interfaces::WinSockStartupCleanupHelper const
OddSource::Interfaces::InterfaceBrowser::_wsa_helper =
        OddSource::Interfaces::WinSockStartupCleanupHelper();
