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
#ifdef IFADDRS4CPP_INCLUDE_BOOST
    using namespace OddSource::Interfaces;

    inline
    IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4
    toV4(
        IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address const & other )
    {
        using namespace ::std::string_literals;
        if ( !other.is_v4() )
        {
            throw InvalidIPAddress(
                "The Boost address provided, "s + other.to_string() +
                ", is not an IPv4 address and cannot be converted to an IPv4Address."s );
        }
        return other.to_v4();
    }

    inline
    IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6
    toV6(
        IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address const & other )
    {
        using namespace ::std::string_literals;
        if ( !other.is_v6() )
        {
            throw InvalidIPAddress(
                "The Boost address provided, "s + other.to_string() +
                ", is not an IPv6 address and cannot be converted to an IPv6Address."s );
        }
        return other.to_v6();
    }
#endif /* IFADDRS4CPP_INCLUDE_BOOST */
}

namespace OddSource::Interfaces
{
#ifdef IFADDRS4CPP_INCLUDE_BOOST
#  ifndef ODDSOURCE_IS_WINDOWS
    inline
    IPv4Address::
    IPv4Address(
        IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address const & other )
        : IPv4Address( toV4( other ) )
    {
    }

    inline
    IPv4Address::
    IPv4Address(
        IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4 const & other )
        : IPv4Address( other.to_uint() )
    {
    }
#  else
    template< class A, ::std::enable_if_t<
        ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address >,
        bool > = true >
    IPv4Address::
    IPv4Address(
        A const & other )
        : IPv4Address( toV4( other ) )
    {
    }

    template< class A, ::std::enable_if_t<
        ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4 >,
        bool > = true >
    IPv4Address::
    IPv4Address(
        A const & other )
        : IPv4Address( other.to_uint() )
    {
    }
#  endif
#endif /* IFADDRS4CPP_INCLUDE_BOOST */

    inline
#if __cplusplus >= 202002L
    constexpr
#endif /* __cplusplus >= 202002L */
    IPAddressVersion
    IPv4Address::
    version() const noexcept
    {
        return IPAddressVersion::IPv4;
    }

    inline
#if __cplusplus >= 202002L
    constexpr
#endif /* __cplusplus >= 202002L */
    ::std::uint16_t
    IPv4Address::
    maximumPrefixLength() const noexcept
    {
        return 32;
    }

    inline
#if __cplusplus >= 202002L
    constexpr
#endif /* __cplusplus >= 202002L */
    size_t
    IPv4Address::
    dataLength() const noexcept
    {
        return 4;
    }

#ifdef IFADDRS4CPP_INCLUDE_BOOST
#  ifndef ODDSOURCE_IS_WINDOWS
    inline
    IPv4Address::
    operator IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address() const
    {
        return IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address(
            static_cast< IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4 >( *this ) );
    }

    inline
    IPv4Address::
    operator IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4() const
    {
        return IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4( static_cast< ::std::uint32_t >( *this ) );
    }
#  else
    template< class A, ::std::enable_if_t<
        ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address >,
        bool > = true >
    inline
    IPv4Address::
    operator A() const
    {
        return IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address(
            static_cast< IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4 >( *this ) );
    }

    template< class A, ::std::enable_if_t<
        ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4 >,
        bool > = true >
    inline
    IPv4Address::
    operator A() const
    {
        return IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v4( static_cast< ::std::uint32_t >( *this ) );
    }
#  endif
#endif /* IFADDRS4CPP_INCLUDE_BOOST */

#ifdef IFADDRS4CPP_INCLUDE_BOOST
#  ifndef ODDSOURCE_IS_WINDOWS
    inline
    IPv6Address::
    IPv6Address(
        IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address const & other )
        : IPv6Address( toV6( other ) )
    {
    }

    inline
    IPv6Address::
    IPv6Address(
        IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6 const & other )
        : IPv6Address( other.to_bytes(), other.scope_id() )
    {
    }
#  else
    template< class A, ::std::enable_if_t<
        ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address >,
        bool > = true >
    IPv6Address::
    IPv6Address(
        A const & other )
        : IPv6Address( toV6( other ) )
    {
    }

    template< class A, ::std::enable_if_t<
        ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6 >,
        bool > = true >
    IPv6Address::
    IPv6Address(
        A const & other )
        : IPv6Address( other.to_bytes(), other.scope_id() )
    {
    }
#  endif
#endif /* IFADDRS4CPP_INCLUDE_BOOST */

    inline
#if __cplusplus >= 202002L
    constexpr
#endif /* __cplusplus >= 202002L */
    IPAddressVersion
    IPv6Address::
    version() const noexcept
    {
        return IPAddressVersion::IPv6;
    }

    inline
#if __cplusplus >= 202002L
    constexpr
#endif /* __cplusplus >= 202002L */
    ::std::uint16_t
    IPv6Address::
    maximumPrefixLength() const noexcept
    {
        return 128;
    }

    inline
#if __cplusplus >= 202002L
    constexpr
#endif /* __cplusplus >= 202002L */
    size_t
    IPv6Address::
    dataLength() const noexcept
    {
        return 16;
    }

#ifdef IFADDRS4CPP_INCLUDE_BOOST
#  ifndef ODDSOURCE_IS_WINDOWS
    inline
    IPv6Address::
    operator IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address() const
    {
        return IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address(
            static_cast< IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6 >( *this ) );
    }

    inline
    IPv6Address::
    operator IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6() const
    {
        return IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6( static_cast< Bytes >( *this ) );
    }
#  else
    template< class A, ::std::enable_if_t<
        ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address >,
        bool > = true >
    inline
    IPv6Address::
    operator A() const
    {
        return IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address(
            static_cast< IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6 >( *this ) );
    }

    template< class A, ::std::enable_if_t<
        ::std::is_same_v< A, IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6 >,
        bool > = true >
    inline
    IPv6Address::
    operator A() const
    {
        return IFADDRS4CPP_BOOST_NAMESPACE_ROOT::asio::ip::address_v6( static_cast< Bytes >( *this ) );
    }
#  endif
#endif /* IFADDRS4CPP_INCLUDE_BOOST */
}
