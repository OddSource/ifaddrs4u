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
#include <netioapi.h>
#include <winsock2.h>
#else /* IS_WINDOWS */
#include <arpa/inet.h>
#include <cerrno>
#include <netdb.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif /* IS_WINDOWS */

#include <cstring>
#include <string>
#include <string_view>
#include <utility>

#include "IpAddress.h"

#define BYTES reinterpret_cast<uint8_t const *>(this->_data.get())
#define WORDS reinterpret_cast<uint16_t const *>(this->_data.get())
#define DOUBLEWORDS reinterpret_cast<uint32_t const *>(this->_data.get())

namespace
{
    using namespace OddSource::Interfaces;

    template<typename Addr>
    using Enable_If_Addr = ::std::enable_if_t<::std::is_same_v<Addr, in_addr> ||
                                              ::std::is_same_v<Addr, in6_addr>>;

    template<typename Addr, typename = Enable_If_Addr<Addr>>
    ::std::unique_ptr<Addr>
    from_repr(::std::string_view const & repr)
    {
        if (repr.length() == 0)
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
            // inet_aton/inet_addr, however, can handle IPv4 addresses in all valid formats.
#ifdef IS_WINDOWS
            auto raw = inet_addr(repr);
        if (raw == INADDR_NONE)
        {
            success = 0;
        }
        else
        {
            ::std::memcpy(data.get(), raw, sizeof(Addr));
        }
#else /* IS_WINDOWS */
            success = inet_aton(repr_str.c_str(), data.get());
#endif
        }
        if (success != 1)
        {
            throw InvalidIPAddress("Malformed IP address string '"s + repr_str + "' or unknown inet_*ton error."s);
        }

        return data;
    }

    template<typename Addr, typename = Enable_If_Addr<Addr>>
    ::std::string
    to_repr(const Addr * data)
    {
        AddressFamily family;
        if constexpr (::std::is_same_v<Addr, in6_addr>)
        {
            family = AF_INET6;
        }
        else
        {
            family = AF_INET;
        }

        static const size_t host_length(100);
        char host_chars[host_length];
        auto ptr = ::inet_ntop(family, data, host_chars, host_length);
        if (ptr == nullptr)
        {
#ifdef IS_WINDOWS
            auto err_no(::WSAGetLastError());
        wchar_t * s = nullptr;
        ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, err_no,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&s, 0, nullptr);
        ::std::string err(
            s == nullptr ?
            ""s :
            ::std::wstring_convert<::std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(::std::wstring(s)));
        LocalFree(s);
#else /* IS_WINDOWS */
            auto err_no(errno);
            char const * err(
                    err_no == EAFNOSUPPORT ? "Address family not supported" :
                    (err_no == ENOSPC ? "Converted address would exceed string size" :
                     ::gai_strerror(errno)));
#endif /* IS_WINDOWS */
            throw InvalidIPAddress(
                "Malformed in_addr data or inet_ntop system error: "s + ::std::to_string(err_no) + " ("s + err + ")"s);
        }
        return host_chars;
    }

    template<typename Addr, typename = Enable_If_Addr<Addr>>
    ::std::string
    to_repr(::std::unique_ptr<Addr> const & data)
    {
        return to_repr(data.get());
    }

    template<typename Addr, typename = Enable_If_Addr<Addr>>
    ::std::unique_ptr<Addr>
    copy_in_addr(Addr const * data)
    {
        auto new_data = ::std::make_unique<Addr>();
        ::std::memcpy(new_data.get(), data, sizeof(Addr));
        return new_data;
    }

    template<typename Addr, typename = Enable_If_Addr<Addr>>
    ::std::unique_ptr<Addr>
    copy_in_addr(::std::unique_ptr<Addr const> const & data)
    {
        return copy_in_addr(data.get());
    }

    inline v6Scope & fill_out_scope(v6Scope & scope)
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
            uint32_t scope_id = ::if_nametoindex(scope.scope_name->c_str());
            if (scope_id > 0)
            {
                scope.scope_id = scope_id;
            }
        }
        return scope;
    }

    inline v6Scope scope_from(uint32_t scope_id)
    {
        if (scope_id == 0)
        {
            throw ::std::invalid_argument("IPv6 address scope ID must be greater than 0.");
        }
        v6Scope scope {scope_id};
        return fill_out_scope(scope);
    }

    inline v6Scope scope_from(::std::string_view const & scope_name)
    {
        if (scope_name.empty())
        {
            throw ::std::invalid_argument("IPv6 address scope name must not be an empty string.");
        }
        v6Scope scope {::std::nullopt, ::std::string(scope_name)};
        return fill_out_scope(scope);
    }
}

OddSource::Interfaces::IPAddress::
IPAddress(::std::string_view const & repr)
    : _representation(repr)
{
}

OddSource::Interfaces::IPAddress::
IPAddress(OddSource::Interfaces::IPAddress const & other) // NOLINT(*-use-equals-default)
    : _representation(other._representation),
      _is_unspecified(other._is_unspecified),
      _is_loopback(other._is_loopback),
      _is_link_local(other._is_link_local),
      _is_private(other._is_private),
      _is_multicast(other._is_multicast),
      _is_reserved(other._is_reserved),
      _multicast_scope(other._multicast_scope)
{
}

OddSource::Interfaces::IPAddress::
IPAddress(OddSource::Interfaces::IPAddress && other) noexcept
    : _representation(other._representation),
      _is_unspecified(other._is_unspecified),
      _is_loopback(other._is_loopback),
      _is_link_local(other._is_link_local),
      _is_private(other._is_private),
      _is_multicast(other._is_multicast),
      _is_reserved(other._is_reserved),
      _multicast_scope(other._multicast_scope)
{
}

OddSource::Interfaces::IPAddress::
~IPAddress() // NOLINT(*-use-equals-default)
{
}

struct OddSource::Interfaces::IPv4TempDataHolder
{
    mutable ::std::unique_ptr<in_addr> data;
};

OddSource::Interfaces::IPv4Address::
IPv4Address(OddSource::Interfaces::IPv4Address const & other)
    : IPAddress(other),
      _data(copy_in_addr(other._data))
{
}

OddSource::Interfaces::IPv4Address::
IPv4Address(::std::string_view const & repr)
    : IPv4Address({from_repr<in_addr>(repr)})
{
}

OddSource::Interfaces::IPv4Address::
IPv4Address(in_addr const * data)
    : IPv4Address({copy_in_addr(data)})
{
}

OddSource::Interfaces::IPv4Address::
IPv4Address(IPv4TempDataHolder const & temp)
    : IPAddress(to_repr(temp.data)),
      _data(::std::move(temp.data))
{
    auto bytes = BYTES;

    if (*reinterpret_cast<uint32_t const *>(this->_data.get()) == 0)
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
            this->_multicast_scope = LinkLocal;
        }
        else if (bytes[0] == 239 && bytes[1] == 255) // 239.255.0.0/16
        {
            this->_multicast_scope = RealmLocal;
        }
        else if (bytes[0] == 239 && bytes[1] >= 192 && bytes[1] <= 195) // 239.192.0.0/14
        {
            this->_multicast_scope = OrganizationLocal;
        }
        else if (bytes[0] != 239) // // 224.0.1.0-238.255.255.255
        {
            this->_multicast_scope = Global;
        }
        else
        {
            this->_multicast_scope = Unassigned;
        }
    }
}

struct OddSource::Interfaces::IPv6TempDataHolder
{
    mutable ::std::unique_ptr<in6_addr> data;
    mutable ::std::optional<v6Scope> scope = ::std::nullopt;
};

OddSource::Interfaces::IPv6Address::
IPv6Address(OddSource::Interfaces::IPv6Address const & other)
    : IPAddress(other),
      _data(copy_in_addr(other._data)),
      _scope(other._scope),
      _without_scope(other._without_scope),
      _is_unique_local(other._is_unique_local),
      _is_site_local(other._is_site_local),
      _is_v4_mapped(other._is_v4_mapped),
      _is_v4_compatible(other._is_v4_compatible),
      _is_v4_translated(other._is_v4_translated),
      _is_6to4(other._is_6to4),
      _multicast_flags(other._multicast_flags)
{
}

OddSource::Interfaces::IPv6Address::
IPv6Address(::std::string_view const & repr)
    : IPv6Address(
        ::std::string(IPv6Address::strip_scope(repr)),
        {from_repr<in6_addr>(IPv6Address::strip_scope(repr)), IPv6Address::extract_scope(repr)})
{
}

OddSource::Interfaces::IPv6Address::
IPv6Address(in6_addr const * data)
    : IPv6Address(to_repr(data), {copy_in_addr(data)})
{
}

OddSource::Interfaces::IPv6Address::
IPv6Address(in6_addr const * data, uint32_t scope_id)
    : IPv6Address(to_repr(data), {copy_in_addr(data), scope_from(scope_id)})
{
}

OddSource::Interfaces::IPv6Address::
IPv6Address(in6_addr const * data, ::std::string_view const & scope_name)
    : IPv6Address(to_repr(data), {copy_in_addr(data), scope_from(scope_name)})
{
}

OddSource::Interfaces::IPv6Address::
IPv6Address(in6_addr const * data, v6Scope const & scope)
    : IPv6Address(to_repr(data), {copy_in_addr(data), scope})
{
}

OddSource::Interfaces::IPv6Address::
IPv6Address(::std::string const & repr, IPv6TempDataHolder const & holder)
    : IPAddress(IPv6Address::add_scope(repr, holder.scope)),
      _data(::std::move(holder.data)),
      _scope(holder.scope ? holder.scope : IPv6Address::extract_scope(repr)),
      _without_scope(repr)
{
    auto bytes = BYTES;
    auto words = WORDS;
    auto doublewords = DOUBLEWORDS;

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
            case 0xf: this->_multicast_scope = Reserved; break;
            case 0x1: this->_multicast_scope = InterfaceLocal; break;
            case 0x2: this->_multicast_scope = LinkLocal; break;
            case 0x3: this->_multicast_scope = RealmLocal; break;
            case 0x4: this->_multicast_scope = AdminLocal; break;
            case 0x5: this->_multicast_scope = SiteLocal; break;
            case 0x8: this->_multicast_scope = OrganizationLocal; break;
            case 0xe: this->_multicast_scope = Global; break;
            default: this->_multicast_scope = Unassigned; break;
        }
    }
}

OddSource::Interfaces::IPv6Address
OddSource::Interfaces::IPv6Address::
normalize() const
{
    if (this->_scope)
    {
        return {this->_data.get(), *this->_scope};
    }
    return {this->_data.get()};
}

::std::string_view
OddSource::Interfaces::IPv6Address::
strip_scope(::std::string_view const & repr)
{
    size_t const i = repr.find('%');
    if (i != ::std::string_view::npos)
    {
        return repr.substr(0, i);
    }
    return repr;
}

::std::optional<OddSource::Interfaces::v6Scope>
OddSource::Interfaces::IPv6Address::
extract_scope(::std::string_view const & repr)
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
                    return scope_from(::std::stoul(scope));
                }
                catch (::std::invalid_argument const &)
                {
                }
            }
            return scope_from(scope);
        }
    }
    return ::std::nullopt;
}

::std::string
OddSource::Interfaces::IPv6Address::
add_scope(::std::string const & repr, ::std::optional<v6Scope> const & scope)
{
    if (!scope)
    {
        return repr;
    }
    return repr + "%" + (scope->scope_name ? *scope->scope_name : ::std::to_string(*scope->scope_id));
}
