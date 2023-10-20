#include "config.h"

#ifdef IS_WINDOWS
#include <netioapi.h>
#include <winsock2.h>
#else /* IS_WINDOWS */
#include <net/if.h>
#include <netinet/in.h>
#endif /* IS_WINDOWS */

#include <cstring>
#include <string>
#include <string_view>
#include <utility>

#include "IpAddress.h"

#define BYTES reinterpret_cast<uint8_t const *>(this->_data)
#define WORDS reinterpret_cast<uint16_t const *>(this->_data)
#define DOUBLEWORDS reinterpret_cast<uint32_t const *>(this->_data)

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

namespace
{
    template<typename Addr, typename = OddSource::Interfaces::Enable_If_Addr<Addr>>
    Addr const *
    copy_in_addr(Addr const * data)
    {
        auto new_data = new Addr;
        ::std::memcpy(new_data, data, sizeof(Addr));
        return new_data;
    }
}

OddSource::Interfaces::IPv4Address::
IPv4Address(OddSource::Interfaces::IPv4Address const & other)
    : IPAddress(other),
      _data(nullptr)
{
    delete this->_data;
    this->_data = copy_in_addr(other._data);
}

OddSource::Interfaces::IPv4Address::
IPv4Address(OddSource::Interfaces::IPv4Address && other) noexcept
    : IPAddress(::std::move(other)),
      _data(nullptr)
{
    ::std::swap(other._data, this->_data); // NOLINT(*-use-after-move)
}

OddSource::Interfaces::IPv4Address::
IPv4Address(::std::string_view const & repr)
    : IPv4Address(IPAddress::from_repr<in_addr>(repr), false)
{
}

OddSource::Interfaces::IPv4Address::
IPv4Address(in_addr const * data)
    : IPv4Address(data, true)
{
}

OddSource::Interfaces::IPv4Address::
IPv4Address(in_addr const * data, bool copy_data)
    : IPv4Address(IPAddress::to_repr(data), data, copy_data)
{
}

OddSource::Interfaces::IPv4Address::
IPv4Address(::std::string_view const & repr, in_addr const * data, bool copy_data)
    : IPAddress(repr),
      _data(copy_data ? copy_in_addr(data) : data)
{
    auto bytes = BYTES;

    if (*reinterpret_cast<uint32_t const *>(this->_data) == 0)
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
             (bytes[0] >= 240 && bytes[0] <= 255) // 240.0.0.0/4
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

OddSource::Interfaces::IPv4Address::
~IPv4Address()
{
    delete this->_data;
}

namespace
{
    using namespace OddSource::Interfaces;

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

OddSource::Interfaces::IPv6Address::
IPv6Address(OddSource::Interfaces::IPv6Address const & other)
    : IPAddress(other),
      _data(nullptr),
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
    delete this->_data;
    this->_data = copy_in_addr(other._data);
}

OddSource::Interfaces::IPv6Address::
IPv6Address(OddSource::Interfaces::IPv6Address && other) noexcept
    : IPAddress(::std::move(other)),
      _data(nullptr),
      _scope(other._scope), // NOLINT(*-use-after-move)
      _without_scope(other._without_scope), // NOLINT(*-use-after-move)
      _is_unique_local(other._is_unique_local), // NOLINT(*-use-after-move)
      _is_site_local(other._is_site_local), // NOLINT(*-use-after-move)
      _is_v4_mapped(other._is_v4_mapped), // NOLINT(*-use-after-move)
      _is_v4_compatible(other._is_v4_compatible), // NOLINT(*-use-after-move)
      _is_v4_translated(other._is_v4_translated), // NOLINT(*-use-after-move)
      _is_6to4(other._is_6to4), // NOLINT(*-use-after-move)
      _multicast_flags(other._multicast_flags) // NOLINT(*-use-after-move)
{
    ::std::swap(other._data, this->_data); // NOLINT(*-use-after-move)
}

OddSource::Interfaces::IPv6Address::
IPv6Address(::std::string_view const & repr)
    : IPv6Address(repr, IPAddress::from_repr<in6_addr>(IPv6Address::strip_scope(repr)), ::std::nullopt, false)
{
}

OddSource::Interfaces::IPv6Address::
IPv6Address(in6_addr const * data)
    : IPv6Address(IPAddress::to_repr(data), data, ::std::nullopt, true)
{
}

OddSource::Interfaces::IPv6Address::
IPv6Address(in6_addr const * data, uint32_t scope_id)
    : IPv6Address(data, scope_from(scope_id))
{
}

OddSource::Interfaces::IPv6Address::
IPv6Address(in6_addr const * data, ::std::string_view const & scope_name)
    : IPv6Address(data, scope_from(scope_name))
{
}

OddSource::Interfaces::IPv6Address::
IPv6Address(in6_addr const * data, v6Scope const & scope)
    : IPv6Address(IPv6Address::add_scope(IPAddress::to_repr(data), scope), data, scope, true)
{
}

OddSource::Interfaces::IPv6Address::
IPv6Address(
    ::std::string_view const & repr,
    in6_addr const * data,
    ::std::optional<v6Scope> const & scope,
    bool copy_data)
    : IPAddress(repr),
      _data(copy_data ? copy_in_addr(data) : data),
      _scope(scope ? scope : IPv6Address::extract_scope(repr)),
      _without_scope(IPv6Address::strip_scope(repr))
{
    auto bytes = BYTES;
    auto words = WORDS;
    auto doublewords = DOUBLEWORDS;

    if (IN6_IS_ADDR_UNSPECIFIED(this->_data))
    {
        this->_is_unspecified = true;
        this->_is_reserved = true;
    }
    else if(IN6_IS_ADDR_LOOPBACK(this->_data))
    {
        this->_is_loopback = true;
        this->_is_reserved = true;
    }
    else if(IN6_IS_ADDR_LINKLOCAL(this->_data) && // some impls erroneously check *only* fe80
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
    else if(IN6_IS_ADDR_SITELOCAL(this->_data))
    {
        this->_is_site_local = true;
        this->_is_private = true;
        this->_is_reserved = true;
    }
    else if(IN6_IS_ADDR_MULTICAST(this->_data))
    {
        this->_is_multicast = true;
        this->_is_reserved = true;
    }
    else if(IN6_IS_ADDR_V4MAPPED(this->_data))
    {
        this->_is_v4_mapped = true;
        this->_is_reserved = true;
    }
    else if(IN6_IS_ADDR_V4COMPAT(this->_data))
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

OddSource::Interfaces::IPv6Address::
~IPv6Address()
{
    delete this->_data;
}

OddSource::Interfaces::IPv6Address
OddSource::Interfaces::IPv6Address::
normalize() const
{
    if (this->_scope)
    {
        return {this->_data, *this->_scope};
    }
    return {this->_data};
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
