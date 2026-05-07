/*
* Copyright © 2010-2026 OddSource Code (license@oddsource.io)
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

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace
{
#ifdef ODDSOURCE_INCLUDE_BOOST
    using namespace OddSource::Interfaces;

    inline
    ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v4
    toV4(
        ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address const & other )
    {
        using namespace std::string_literals;
        if ( !other.is_v4() )
        {
            throw InvalidIPAddress(
                "The Boost address provided, "s + other.to_string() +
                ", is not an IPv4 address and cannot be converted to an IPv4Address."s );
        }
        return other.to_v4();
    }

    inline
    ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v6
    toV6(
        ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address const & other )
    {
        using namespace std::string_literals;
        if ( !other.is_v6() )
        {
            throw InvalidIPAddress(
                "The Boost address provided, "s + other.to_string() +
                ", is not an IPv6 address and cannot be converted to an IPv6Address."s );
        }
        return other.to_v6();
    }
#endif /* ODDSOURCE_INCLUDE_BOOST */
}

namespace OddSource::Interfaces
{
#ifdef ODDSOURCE_INCLUDE_BOOST
    inline
    IPv4Address::
    IPv4Address(
        ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address const & other )
        : IPv4Address( toV4( other ) )
    {
    }

    inline
    IPv4Address::
    IPv4Address(
        ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v4 const & other )
        : IPv4Address( other.to_uint() )
    {
    }
#endif /* ODDSOURCE_INCLUDE_BOOST */

    inline
    IPAddressVersion
    IPv4Address::
    version() const noexcept
    {
        return IPAddressVersion::IPv4;
    }

    inline
    ::std::uint8_t
    IPv4Address::
    maximum_prefix_length() const noexcept
    {
        return 32;
    }

#ifdef ODDSOURCE_INCLUDE_BOOST
    inline
    IPv4Address::
    operator ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address() const
    {
        return ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address(
            static_cast< ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v4 >( *this ) );
    }

    inline
    IPv4Address::
    operator ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v4() const
    {
        return ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v4( static_cast< ::std::uint32_t >( *this ) );
    }

    inline
    IPv6Address::
    IPv6Address(
        ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address const & other )
        : IPv6Address( toV6( other ) )
    {
    }

    inline
    IPv6Address::
    IPv6Address(
        ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v6 const & other )
        : IPv6Address( other.to_bytes(), other.scope_id() )
    {
    }
#endif /* ODDSOURCE_INCLUDE_BOOST */

    inline
    IPAddressVersion
    IPv6Address::
    version() const noexcept
    {
        return IPAddressVersion::IPv6;
    }

    inline
    ::std::uint8_t
    IPv6Address::
    maximum_prefix_length() const noexcept
    {
        return 128;
    }

#ifdef ODDSOURCE_INCLUDE_BOOST
    inline
    IPv6Address::
    operator ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address() const
    {
        return ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address(
            static_cast< ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v6 >( *this ) );
    }

    inline
    IPv6Address::
    operator ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v6() const
    {
        return ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v6( static_cast< Bytes >( *this ) );
    }
#endif /* ODDSOURCE_INCLUDE_BOOST */
}
