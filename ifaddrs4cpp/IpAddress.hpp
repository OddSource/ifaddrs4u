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

#ifdef IS_WINDOWS
#include <ws2tcpip.h>
#else /* IS_WINDOWS */
#include <netinet/in.h>
#endif /* IS_WINDOWS */

#include <sstream>

inline
OddSource::Interfaces::IPAddress::
operator ::std::string() const
{
    return this->_representation;
}

inline
OddSource::Interfaces::IPAddress::
operator char const *() const
{
    return this->_representation.c_str();
}

inline size_t
OddSource::Interfaces::IPv4Address::
data_length() const
{
    return 4;
}

inline
OddSource::Interfaces::IPv4Address::
operator in_addr const *() const
{
    return this->_data.get();
}

inline size_t
OddSource::Interfaces::IPv6Address::
data_length() const
{
    return 16;
}

inline
OddSource::Interfaces::IPv6Address::
operator in6_addr const *() const
{
    return this->_data.get();
}

inline bool
OddSource::Interfaces::IPAddress::
is_unspecified() const
{
    return this->_is_unspecified;
}

inline bool
OddSource::Interfaces::IPAddress::
is_loopback() const
{
    return this->_is_loopback;
}

inline bool
OddSource::Interfaces::IPAddress::
is_link_local() const
{
    return this->_is_link_local;
}

inline bool
OddSource::Interfaces::IPAddress::
is_private() const
{
    return this->_is_private;
}

inline bool
OddSource::Interfaces::IPAddress::
is_multicast() const
{
    return this->_is_multicast;
}

inline bool
OddSource::Interfaces::IPAddress::
is_reserved() const
{
    return this->_is_reserved;
}

inline ::std::optional<OddSource::Interfaces::MulticastScope> const &
OddSource::Interfaces::IPAddress::
multicast_scope() const
{
    return this->_multicast_scope;
}

inline OddSource::Interfaces::IPAddressVersion
OddSource::Interfaces::IPv4Address::
version() const
{
    return IPv4;
}

inline uint8_t
OddSource::Interfaces::IPv4Address::
maximum_prefix_length() const
{
    return 32;
}

inline bool
OddSource::Interfaces::IPv4Address::
operator==(OddSource::Interfaces::IPv4Address const & other) const
{
    return (*reinterpret_cast<uint32_t const *>(this->_data.get()) ==
            *reinterpret_cast<uint32_t const *>(other._data.get()));
}

inline bool
OddSource::Interfaces::IPv4Address::
operator!=(OddSource::Interfaces::IPv4Address const & other) const
{
    return !this->operator==(other);
}

inline OddSource::Interfaces::IPAddressVersion
OddSource::Interfaces::IPv6Address::
version() const
{
    return IPv6;
}

inline uint8_t
OddSource::Interfaces::IPv6Address::
maximum_prefix_length() const
{
    return 128;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_unique_local() const
{
    return this->_is_unique_local;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_site_local() const
{
    return this->_is_site_local;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_v4_mapped() const
{
    return this->_is_v4_mapped;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_v4_translated() const
{
    return this->_is_v4_translated;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_v4_compatible() const
{
    return this->_is_v4_compatible;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_6to4() const
{
    return this->_is_6to4;
}

inline bool
OddSource::Interfaces::IPv6Address::
has_scope_id() const
{
    return (bool)this->_scope;
}

inline ::std::string
OddSource::Interfaces::IPv6Address::
without_scope_id() const
{
    return this->_without_scope;
}

inline ::std::optional<uint32_t> const &
OddSource::Interfaces::IPv6Address::
scope_id() const
{
    static ::std::optional<uint32_t> const nil; // prevent "returning ref to temp local"
    return this->_scope ? this->_scope->scope_id : nil;
}

inline ::std::optional<::std::string> const &
OddSource::Interfaces::IPv6Address::
scope_name() const
{
    static ::std::optional<::std::string> const nil; // prevent "returning ref to temp local"
    return this->_scope ? this->_scope->scope_name : nil;
}

inline ::std::optional<::std::string>
OddSource::Interfaces::IPv6Address::
scope_name_or_id() const
{
    if (!this->_scope)
    {
        return ::std::nullopt;
    }
    return this->_scope->scope_name ? this->_scope->scope_name : ::std::to_string(*this->_scope->scope_id);
}

inline ::std::optional<::std::string>
OddSource::Interfaces::IPv6Address::
scope_id_or_name() const
{
    if (!this->_scope)
    {
        return ::std::nullopt;
    }
    return this->_scope->scope_id ? ::std::to_string(*this->_scope->scope_id) : this->_scope->scope_name;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_multicast_flag_enabled(OddSource::Interfaces::MulticastV6Flag flag) const
{
    return this->_multicast_flags && (*this->_multicast_flags & flag) == flag;
}

inline bool
OddSource::Interfaces::IPv6Address::
operator==(OddSource::Interfaces::IPv6Address const & other) const
{
    auto data1 = reinterpret_cast<uint8_t const *>(this->_data.get());
    auto data2 = reinterpret_cast<uint8_t const *>(other._data.get());
    auto length = this->data_length();
    for(size_t i(0); i < length; i++)
    {
        if (data1[i] != data2[i])
        {
            return false;
        }
    }
    return true;
}

inline bool
OddSource::Interfaces::IPv6Address::
operator!=(OddSource::Interfaces::IPv6Address const & other) const
{
    return !this->operator==(other);
}

inline ::std::ostream &
OddSource::Interfaces::
operator<<(::std::ostream & os, OddSource::Interfaces::IPAddress const & address)
{
    return os << ::std::string(address);
}
