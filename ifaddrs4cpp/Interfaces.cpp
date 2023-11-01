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

#include "config.h"
#include "s.h"

#ifdef IS_WINDOWS
#include "winsock_includes.h"
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
#define HAS_LINUX_IPV6
#endif /* <linux/ipv6.h> */
#if __has_include(<net/if_arp.h>)
#include <net/if_arp.h>
#endif
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
#include <mutex>
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

#ifdef IS_WINDOWS
std::string utf8_encode(std::wstring const & wstr)
{
    if (wstr.empty())
    {
        return std::string();
    }
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &str[0], size_needed, NULL, NULL);
    return str;
}
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
                        "Call to GetAdaptersAddresses failed with error code: "s + ::std::to_string(dwRetVal));
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
                if (has_broadcast(ifa->FirstPrefix))
                {
                    flags |= BroadcastAddressSet;
                }
                if ((ifa->Flags & IP_ADAPTER_NO_MULTICAST) != IP_ADAPTER_NO_MULTICAST)
                {
                    flags |= SupportsMulticast;
                }
                ::std::string uuid(ifa->AdapterName);
                Interface iface(
                    ifa->IfIndex,
                    ::std::wstring_convert<::std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(ifa->FriendlyName),
                    uuid.substr(1, uuid.length() - 2), // strip {}
                    flags,
                    ifa->Mtu);

                if (ifa->PhysicalAddress)
                {
                    iface._mac_address.emplace(ifa->PhysicalAddress, ifa->PhysicalAddressLength);
                }

                PIP_ADAPTER_UNICAST_ADDRESS unicast = ifa->FirstUnicastAddress;
                while (unicast)
                {
                    LPSOCKADDR sa = unicast->Address.lpSockaddr;
                    if (sa->sa_family == AF_INET)
                    {
                        InterfacesHelper::add_ipv4_address(
                                sa, ifa->FirstPrefix, iface, unicast->OnLinkPrefixLength);
                    }
                    else if (sa->sa_family == AF_INET6)
                    {
                        uint16_t addr_flags(0);
                        if (unicast->SuffixOrigin == IpSuffixOriginRandom)
                        {
                            addr_flags |= Temporary;
                        }
                        InterfacesHelper::add_ipv6_address(
                                sa, iface, unicast->OnLinkPrefixLength, addr_flags);
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
                        InterfacesHelper::add_ipv4_address(sa, ifa->FirstPrefix, iface);
                    }
                    else if (sa->sa_family == AF_INET6)
                    {
                        uint16_t addr_flags(Anycast);
                        InterfacesHelper::add_ipv6_address(sa, iface, 0, addr_flags);
                    }
                    anycast = anycast->Next;
                }

                if (!do_this(iface))
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
            ifaddrs * ifa = this->_ifaddrs;
            ::std::unordered_map<::std::string, Interface> interfaces;
            ::std::unordered_map<uint32_t, ::std::string> indexes_to_names;
            while (ifa)
            {
                ::std::string name(ifa->ifa_name);
                auto found = interfaces.find(name);
                if (found == interfaces.end())
                {
                    uint32_t index(::if_nametoindex(ifa->ifa_name));
                    assert(index > 0);
                    interfaces.emplace(
                        name,
                        Interface(index, name, ifa->ifa_flags, InterfacesHelper::get_mtu(ifa->ifa_name)));
                    indexes_to_names.emplace(index, name);
                    found = interfaces.find(name);
                }

                Interface & iface = found->second;
                if (ifa->ifa_flags != iface._flags)
                {
                    // TODO what should we do in this case? Does this mean the OS is misbehaving?
                    ::std::cerr << "Flags " << ::std::to_string(ifa->ifa_flags)
                                << " for next item in interface " << name
                                << " does not match first flags " << ::std::to_string(iface._flags)
                                << ::std::endl;
                }

                if (ifa->ifa_addr)
                {
                    AddressFamily family(ifa->ifa_addr->sa_family);
                    if (family == AF_MAC_ADDRESS)
                    {
                        InterfacesHelper::set_mac_address(ifa, iface);
                    }
                    else if (family == AF_INET)
                    {
                        InterfacesHelper::add_ipv4_address(ifa, iface);
                    }
                    else if (family == AF_INET6)
                    {
                        InterfacesHelper::add_ipv6_address(ifa, iface, indexes_to_names);
                    }
                    else
                    {
                        ::std::cerr << "Unrecognized address family " << ::std::to_string(family)
                                    << " on interface " << name << ::std::endl;
                    }
                }

                ifa = ifa->ifa_next;
            }

            for (auto & [name, iface] : interfaces) // NOLINT(*-use-anyofallof)
            {
                if (!do_this(iface))
                {
                    return false;
                }
            }

            return true;
#endif /* !IS_WINDOWS */
        }

    private:
#ifdef IS_WINDOWS
        static bool has_broadcast(PIP_ADAPTER_PREFIX pre)
        {
            while (pre)
            {
                LPSOCKADDR candidate = pre->Address.lpSockaddr;
                if (candidate->sa_family == AF_INET)
                {
                    auto cand = reinterpret_cast<sockaddr_in *>(candidate);
                    auto cand_bytes = reinterpret_cast<uint8_t *>(cand->sin_addr.s_addr);
                    uint8_t i, first_byte_with_bcast(0);
                    for (i = 3; i >= 0; i--)
                    {
                        if (cand_bytes[i] == 0xff)
                        {
                            first_byte_with_bcast = i;
                        }
                    }
                    if (first_byte_with_bcast >= 1)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        static void add_ipv4_address(
            LPSOCKADDR sa, PIP_ADAPTER_PREFIX pre, Interface & iface,
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
                    auto cand_bytes = reinterpret_cast<uint8_t *>(cand.sin_addr.s_addr);
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
                    auto addr_bytes = reinterpret_cast<uint8_t *>(addr.sin_addr.s_addr);
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
                iface._ipv4_addresses.emplace_back(
                    address, flags, prefix_length,
                    IPv4Address(&broadcast->sin_addr));
            }
            else
            {
                iface._ipv4_addresses.emplace_back(address, flags, prefix_length);
            }
        }

        static void add_ipv6_address(
            LPSOCKADDR sa, Interface & iface,
            uint8_t prefix_length = 0, uint16_t flags = 0)
        {
            auto addr = reinterpret_cast<sockaddr_in6 *>(sa);
            if (addr->sin6_scope_id)
            {
                IPv6Address const address(&addr->sin6_addr, addr->sin6_scope_id);
                iface._ipv6_addresses.emplace_back(address, flags, prefix_length);
            }
            else
            {
                IPv6Address const address(&addr->sin6_addr);
                iface._ipv6_addresses.emplace_back(address, flags, prefix_length);
            }
        }

        PIP_ADAPTER_ADDRESSES _pAddresses;
#else /* IS_WINDOWS */
        static ::std::optional<uint64_t const> get_mtu(char const * if_name)
        {
            ::std::optional<uint64_t const> mtu;
#ifdef SIOCGIFMTU
            ifreq ifr {};
            ::strncpy(ifr.ifr_name, if_name, IFNAMSIZ - 1);
            int sock(::socket(AF_INET, SOCK_DGRAM, 0));
            if (sock < 0)
            {
                // TODO print error?
            }
            else if (::ioctl(sock, SIOCGIFMTU, &ifr) >= 0)
            {
                mtu.emplace(ifr.ifr_mtu);
            }
#endif
            return mtu;
        }

        static void set_mac_address(ifaddrs *ifa, Interface & iface)
        {
#ifdef AF_LINK
            assert(ifa->ifa_addr->sa_family == AF_LINK);
            auto addr = reinterpret_cast<sockaddr_dl *>(ifa->ifa_addr);
            // LLADDR returns a signed char, not unsigned, but the data itself is
            // actually unsigned. (This is because addr->sdl_data contains two sets
            // of data in a signed char, and LLADDR merely extracts the one we need.)
            // So we have to cast to unsigned in order to make use of the data.
            auto data_length = addr->sdl_alen; // should always be 6, but you never know
            if (data_length < MIN_ADAPTER_ADDRESS_LENGTH)
            {
                return;
            }
            auto data = reinterpret_cast<uint8_t const *>(LLADDR(addr));
#else /* AF_LINK */
            assert(ifa->ifa_addr->sa_family == AF_PACKET);
            auto addr = reinterpret_cast<sockaddr_ll *>(ifa->ifa_addr);
            if (addr->sll_hatype != ARPHRD_ETHER)
            {
                return;
            }
            auto data_length = addr->sll_halen; // should always be 6, but you never know
            if (data_length < MIN_ADAPTER_ADDRESS_LENGTH)
            {
                return;
            }
            auto data = addr->sll_addr;
#endif /* !AF_LINK */
            for (uint8_t i(0); i < data_length; i++)
            {
                if (data[i] > 0)
                {
                    // make sure at least one byte is nonzero
                    iface._mac_address.emplace(data, data_length);
                    return;
                }
            }
        }

        static void add_ipv4_address(ifaddrs *ifa, Interface & iface)
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
            if (iface.is_flag_enabled(BroadcastAddressSet) && ifa->ifa_broadaddr)
            {
                auto broadcast = reinterpret_cast<sockaddr_in *>(ifa->ifa_broadaddr);
                iface._ipv4_addresses.emplace_back(
                    address, flags, prefix_length,
                    IPv4Address(&broadcast->sin_addr));
            }
            else if(iface.is_flag_enabled(IsPointToPoint) && ifa->ifa_dstaddr)
            {
                auto point_to_point = reinterpret_cast<sockaddr_in *>(ifa->ifa_dstaddr);
                iface._ipv4_addresses.emplace_back(
                    address, flags, prefix_length,
                    IPv4Address(&point_to_point->sin_addr), true);
            }
            else
            {
                iface._ipv4_addresses.emplace_back(address, flags, prefix_length);
            }
        }

        static void add_ipv6_address(
            ifaddrs *ifa,
            Interface & iface,
            ::std::unordered_map<uint32_t, ::std::string> & indexes_to_names)
        {
            auto addr = reinterpret_cast<sockaddr_in6 *>(ifa->ifa_addr);
            ::std::optional<v6Scope> scope;
            ::std::optional<uint32_t> scope_id;
            if (addr->sin6_scope_id)
            {
                auto found_name = indexes_to_names.find(addr->sin6_scope_id);
                if (found_name != indexes_to_names.end())
                {
                    scope = {addr->sin6_scope_id, found_name->second};
                }
                else
                {
                    scope_id = addr->sin6_scope_id;
                }
            }

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
            ::strncpy(ifr6.ifr_name, ifa->ifa_name, IFNAMSIZ - 1);
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

            if (scope)
            {
                iface._ipv6_addresses.emplace_back(IPv6Address(&addr->sin6_addr, *scope), flags, prefix_length);
            }
            else if (scope_id)
            {
                iface._ipv6_addresses.emplace_back(IPv6Address(&addr->sin6_addr, *scope_id), flags, prefix_length);
            }
            else
            {
                iface._ipv6_addresses.emplace_back(IPv6Address(&addr->sin6_addr), flags, prefix_length);
            }
        }

        struct ifaddrs * _ifaddrs;
#endif /* !IS_WINDOWS */
    };
}

OddSource::Interfaces::InterfaceBrowser::
InterfaceBrowser()
    : _storage_mutex(),
      _storage_filled(false),
      _interface_vector(),
      _index_map(),
      _name_map()
{
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

::std::vector<OddSource::Interfaces::Interface> const &
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
        throw ::std::invalid_argument("No interface found with index: "s + ::std::to_string(index));
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
        throw ::std::invalid_argument("No interface found with name: "s + ::std::string(name));
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
    ::std::vector<Interface> temp_vector;
    ::std::unordered_map<uint32_t, ::std::shared_ptr<Interface const>> temp_index_map;
    ::std::unordered_map<::std::string, ::std::shared_ptr<Interface const>> temp_name_map;
    ::std::function<bool(Interface &)> do_this_internal = [&](auto iface)
    {
        if (return_internal && do_this)
        {
            return_internal = (*do_this)(iface);
        }
        auto ptr = ::std::make_shared<Interface const>(iface);
        temp_index_map.insert(std::make_pair(iface.index(), ptr));
        temp_name_map.insert(std::make_pair(iface.name(), ptr));
#if IS_WINDOWS
        temp_name_map.insert(std::make_pair(iface.windows_uuid(), ptr));
        temp_name_map.insert(std::make_pair("{"s + iface.windows_uuid() + "}"s, ptr));
#endif /* IS_WINDOWS */
        temp_vector.push_back(::std::move(iface));

        return true;
    };
    InterfacesHelper().for_each_interface(do_this_internal);
    this->_interface_vector = ::std::move(temp_vector);
    this->_index_map = ::std::move(temp_index_map);
    this->_name_map = ::std::move(temp_name_map);
    this->_storage_filled = true;
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
        throw ::std::runtime_error(
            "Could not initialize WinSock subsystem due to error code: "s +
            ::std::to_string(error) + ". For the meaning of this, see the documentation: "s +
            "https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup#return-value"s);
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
