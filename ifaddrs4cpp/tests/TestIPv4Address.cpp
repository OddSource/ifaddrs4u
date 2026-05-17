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

#include <oddsource/network/interfaces/IpAddress.hpp>
#include "main.h"

#include <cstring>

#ifdef ODDSOURCE_IS_WINDOWS
#include <netioapi.h>
#else /* ODDSOURCE_IS_WINDOWS */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif /* ODDSOURCE_IS_WINDOWS */

using namespace OddSource::Interfaces;

class TestIPv4Address : public Tests::Test
{
public:
    TestIPv4Address()
    {
        add_test( test_equals );
        add_test( test_string_round_trip );
        add_test( test_in_addr_round_trip );
        add_test( test_uint_round_trip );
        add_test( test_unspecified_address );
        add_test( test_loopback_addresses );
        add_test( test_link_local_addresses );
        add_test( test_multicast_addresses );
        add_test( test_private_addresses );
        add_test( test_other_reserved_addresses );
        add_test( test_construct_malformed );
        add_test( test_construct_either_version );

#ifdef ODDSOURCE_INCLUDE_BOOST
        add_test( test_boost_address_conversion );
#endif
    }

    TestIPv4Address(
        TestIPv4Address const & ) = delete;

    TestIPv4Address(
        TestIPv4Address && ) = delete;

    TestIPv4Address &
    operator=(
        TestIPv4Address const & ) = delete;

    TestIPv4Address &
    operator=(
        TestIPv4Address && ) = delete;

    void
    test_equals()
    {
        assert_equals( IPv4Address( "4.3.5.6" ), IPv4Address( "4.3.5.6" ) );
        assert_equals( IPv4Address( "4.3.5.6" ), IPv4Address( "0x04.0x03.0x05.0x06" ) );
        assert_not_equals( IPv4Address( "172.19.52.141" ), IPv4Address( "172.19.52.140" ) );
    }

    void
    test_string_round_trip()
    {
        assert_equals( static_cast< ::std::string >( IPv4Address( "4.3.5.6" ) ), "4.3.5.6" );
        assert_equals(
            ::strcmp( static_cast< char const * >( IPv4Address( "4.3.5.6" ) ), "4.3.5.6" ),
            0,
            "The C strings do not match." );

        assert_equals( static_cast< ::std::string >( IPv4Address( "226.000.000.037" ) ), "226.0.0.31" ); // octal
        assert_equals( static_cast< ::std::string >( IPv4Address( "0x11.0x1b.0xf3.0x01" ) ), "17.27.243.1" ); // hexadecimal

        ::std::ostringstream oss;
        oss << IPv4Address( "172.19.52.141" );
        assert_equals( oss.str(), "172.19.52.141" );
    }

    void
    test_in_addr_round_trip()
    {
        using namespace ::std::string_literals;
        in_addr data {};
        inet_pton( AF_INET, "192.0.2.33", &data );
        IPv4Address const address( &data );
        assert_equals( static_cast< ::std::string >( address ), "192.0.2.33" );

        assert_equals( address.version(), IPAddressVersion::IPv4 );
        assert_equals( address.maximumPrefixLength(), 32 );

        auto owned( static_cast< in_addr const * >( address ) );
        auto bytes1( reinterpret_cast< uint8_t const * >( owned ) );
        auto bytes2( reinterpret_cast< uint8_t const * >( &data ) );

        for( size_t i{ 0 }; i < sizeof( in_addr ); i++ )
        {
            assert_equals( bytes1[ i ], bytes2[ i ], "Bytes "s + ::std::to_string( i ) + " do not match."s );
        }
    }

    void
    test_uint_round_trip()
    {
        IPv4Address const address( 1767959308u );
        assert_equals( static_cast< ::std::string >( address ), "105.96.235.12" );
        assert_equals( static_cast< ::std::uint32_t >( address ), 1767959308u );
    }

    void
    test_unspecified_address()
    {
        IPv4Address const address( "0.0.0.0" );
        assert_that( address.isUnspecified(), "0.0.0.0 should be unspecified." );
        assert_that( address.isReserved(), "0.0.0.0 should be reserved." );
        assert_that( !address.isPrivate(), "0.0.0.0 should not be private." );
        assert_that( !address.isLoopback(), "0.0.0.0 should not be a loopback." );
        assert_that( !address.isLinkLocal(), "0.0.0.0 should not be link-local." );
        assert_that( !address.isMulticast(), "0.0.0.0 should not be multicast." );

        assert_that( !IPv4Address( "0.0.0.1" ).isUnspecified(), "0.0.0.1 should not be unspecified." );
    }

    void
    test_loopback_addresses()
    {
        IPv4Address const address( "127.0.0.0" );
        assert_that( address.isLoopback(), "127.0.0.0 should be a loopback." );
        assert_that( address.isReserved(), "127.0.0.0 should be reserved." );
        assert_that( !address.isPrivate(), "127.0.0.0 should not be private." );
        assert_that( !address.isUnspecified(), "127.0.0.0 should not be a unspecified." );
        assert_that( !address.isLinkLocal(), "127.0.0.0 should not be link-local." );
        assert_that( !address.isMulticast(), "127.0.0.0 should not be multicast." );

        assert_that( IPv4Address( "127.0.0.1" ).isLoopback(), "127.0.0.1 should be a loopback." );
        assert_that( IPv4Address( "127.0.0.124" ).isLoopback(), "127.0.0.124 should be a loopback." );
        assert_that( IPv4Address( "127.0.0.255" ).isLoopback(), "127.0.0.255 should be a loopback." );
        assert_that( IPv4Address( "127.255.255.255" ).isLoopback(), "127.255.255.255 should be a loopback." );
    }

    void
    test_link_local_addresses()
    {
        IPv4Address const address( "169.254.0.0" );
        assert_that( address.isLinkLocal(), "169.254.0.0 should be link-local." );
        assert_that( address.isReserved(), "169.254.0.0 should be reserved." );
        assert_that( !address.isPrivate(), "169.254.0.0 should not be private." );
        assert_that( !address.isUnspecified(), "169.254.0.0 should not be unspecified." );
        assert_that( !address.isLoopback(), "169.254.0.0 should not be a loopback." );
        assert_that( !address.isMulticast(), "169.254.0.0 should not be multicast." );

        assert_that( IPv4Address( "169.254.0.1" ).isLinkLocal(), "169.254.0.1 should be link-local." );
        assert_that( IPv4Address( "169.254.0.124" ).isLinkLocal(), "169.254.0.124 should be link-local." );
        assert_that( IPv4Address( "169.254.0.255" ).isLinkLocal(), "169.254.0.255 should be link-local." );
        assert_that( IPv4Address( "169.254.255.255" ).isLinkLocal(), "169.254.255.255 should be link-local." );
    }

    void
    test_multicast_addresses()
    {
        IPv4Address const address( "224.0.0.0" );
        assert_that( address.isMulticast(), "224.0.0.0 should be multicast." );
        assert_that( address.isReserved(), "224.0.0.0 should be reserved." );
        assert_that( !address.isPrivate(), "224.0.0.0 should not be private." );
        assert_that( !address.isUnspecified(), "224.0.0.0 should not be unspecified." );
        assert_that( !address.isLoopback(), "224.0.0.0 should not be a loopback." );
        assert_that( !address.isLinkLocal(), "224.0.0.0 should not be link-local." );

        assert_that( IPv4Address( "224.0.0.1" ).isMulticast(), "224.0.0.1 should be multicast." );
        assert_that( IPv4Address( "224.0.0.124" ).isMulticast(), "224.0.0.124 should be multicast." );
        assert_that( IPv4Address( "224.0.0.255" ).isMulticast(), "224.0.0.255 should be multicast." );
        assert_that( IPv4Address( "239.0.0.1" ).isMulticast(), "239.0.0.1 should be multicast." );
        assert_that( IPv4Address( "239.255.255.255" ).isMulticast(), "239.255.255.255 should be multicast." );

        assert_that(
            !IPv4Address( "192.168.0.1" ).multicastScope(),
            "192.168.0.1 should not have a multicast scope." );
        assert_equals( *IPv4Address( "224.0.0.0" ).multicastScope(), MulticastScope::LinkLocal );
        assert_equals( *IPv4Address( "224.0.0.255" ).multicastScope(), MulticastScope::LinkLocal );
        assert_equals( *IPv4Address( "224.0.1.0" ).multicastScope(), MulticastScope::Global );
        assert_equals( *IPv4Address( "238.255.255.255" ).multicastScope(), MulticastScope::Global );
        assert_equals( *IPv4Address( "239.0.0.0" ).multicastScope(), MulticastScope::Unassigned );
        assert_equals( *IPv4Address( "239.191.255.255" ).multicastScope(), MulticastScope::Unassigned );
        assert_equals( *IPv4Address( "239.192.0.0" ).multicastScope(), MulticastScope::OrganizationLocal );
        assert_equals( *IPv4Address( "239.192.255.255" ).multicastScope(), MulticastScope::OrganizationLocal );
        assert_equals( *IPv4Address( "239.195.255.255" ).multicastScope(), MulticastScope::OrganizationLocal );
        assert_equals( *IPv4Address( "239.196.0.0" ).multicastScope(), MulticastScope::Unassigned );
        assert_equals( *IPv4Address( "239.254.255.255" ).multicastScope(), MulticastScope::Unassigned );
        assert_equals( *IPv4Address( "239.255.0.0" ).multicastScope(), MulticastScope::RealmLocal );
        assert_equals( *IPv4Address( "239.255.255.255" ).multicastScope(), MulticastScope::RealmLocal );
    }

    void
    test_private_addresses()
    {
        IPv4Address const address( "10.0.0.0" );
        assert_that( address.isPrivate(), "10.0.0.0 should be private" );
        assert_that( address.isReserved(), "10.0.0.0 should be reserved." );
        assert_that( !address.isMulticast(), "10.0.0.0 should not be multicast." );
        assert_that( !address.isUnspecified(), "10.0.0.0 should not be unspecified." );
        assert_that( !address.isLoopback(), "10.0.0.0 should not be a loopback." );
        assert_that( !address.isLinkLocal(), "10.0.0.0 should not be link-local." );

        assert_that( !IPv4Address( "9.255.255.255" ).isPrivate(), "9.255.255.255 should not be private." );
        assert_that( IPv4Address( "10.0.0.1" ).isPrivate(), "10.0.0.1 should be private." );
        assert_that( IPv4Address( "10.20.30.75" ).isPrivate(), "10.20.30.75 should be private." );
        assert_that( IPv4Address( "10.255.255.255" ).isPrivate(), "10.255.255.255 should be private." );
        assert_that( !IPv4Address( "11.0.0.0" ).isPrivate(), "11.0.0.0 should be not private." );

        assert_that( !IPv4Address( "100.63.255.255" ).isPrivate(), "100.63.255.255 should not be private." );
        assert_that( IPv4Address( "100.64.0.0" ).isPrivate(), "100.64.0.0 should be private." );
        assert_that( IPv4Address( "100.99.30.15" ).isPrivate(), "100.99.30.15 should be private." );
        assert_that( IPv4Address( "100.127.255.255" ).isPrivate(), "100.127.255.255 should be private." );
        assert_that( !IPv4Address( "100.128.0.0" ).isPrivate(), "100.128.0.0 should not be private." );

        assert_that( !IPv4Address( "172.15.255.255" ).isPrivate(), "172.15.255.255 should not be private." );
        assert_that( IPv4Address( "172.16.0.0" ).isPrivate(), "172.16.0.0 should be private." );
        assert_that( IPv4Address( "172.24.5.5" ).isPrivate(), "172.24.5.5 should be private." );
        assert_that( IPv4Address( "172.31.255.255" ).isPrivate(), "172.31.255.255 should be private." );
        assert_that( !IPv4Address( "172.32.0.0" ).isPrivate(), "172.32.0.0 should not be private." );

        assert_that( !IPv4Address( "191.255.255.255" ).isPrivate(), "191.255.255.255 should not be private." );
        assert_that( IPv4Address( "192.0.0.0" ).isPrivate(), "192.0.0.0 should be private." );
        assert_that( IPv4Address( "192.0.0.255" ).isPrivate(), "192.0.0.255 should be private." );
        assert_that( !IPv4Address( "192.0.1.0" ).isPrivate(), "192.0.1.0 should not be private." );

        assert_that( !IPv4Address( "192.167.255.255" ).isPrivate(), "192.167.255.255 should not be private." );
        assert_that( IPv4Address( "192.168.0.0" ).isPrivate(), "192.168.0.0 should be private." );
        assert_that( IPv4Address( "192.168.255.255" ).isPrivate(), "192.168.255.255 should be private." );
        assert_that( !IPv4Address( "192.169.0.0" ).isPrivate(), "192.169.0.0 should not be private." );

        assert_that( !IPv4Address( "198.17.255.255" ).isPrivate(), "198.17.255.255 should not be private." );
        assert_that( IPv4Address( "198.18.0.0" ).isPrivate(), "198.18.0.0 should be private." );
        assert_that( IPv4Address( "198.18.255.255" ).isPrivate(), "198.18.255.255 should be private." );
        assert_that( IPv4Address( "198.19.0.0" ).isPrivate(), "198.19.0.0 should be private." );
        assert_that( IPv4Address( "198.19.255.255" ).isPrivate(), "198.19.255.255 should be private." );
        assert_that( !IPv4Address( "198.20.0.0" ).isPrivate(), "198.20.0.0 should not be private." );
    }

    void
    test_other_reserved_addresses()
    {
        static ::std::vector< ::std::string > const tests {
            "192.0.2.0", "192.0.2.255",
            "192.88.99.0", "192.88.99.255",
            "198.51.100.0", "198.51.100.255",
            "203.0.113.0", "203.0.113.255",
            "240.0.0.0", "240.0.0.255", "240.0.255.255", "240.255.255.255",
            "248.0.0.0", "250.0.0.0", "252.0.0.0", "254.0.0.0",
            "255.255.255.254", "255.255.255.255",
        };
        for ( auto const & test : tests )
        {
            IPv4Address const address( test );
            assert_that( address.isReserved(), test + " should be reserved." );
            assert_that( !address.isPrivate(), test + " should not be private" );
            assert_that( !address.isMulticast(), test + " should not be multicast." );
            assert_that( !address.isUnspecified(), test + " should not be unspecified." );
            assert_that( !address.isLoopback(), test + " should not be a loopback." );
            assert_that( !address.isLinkLocal(), test + " should not be link-local." );
        }
    }

    void
    test_construct_malformed()
    {
        assert_throws( IPv4Address( "" ), InvalidIPAddress );
        assert_throws( IPv4Address( "192" ), InvalidIPAddress );
        assert_throws( IPv4Address( "192.168" ), InvalidIPAddress );
        assert_throws( IPv4Address( "192.168.0" ), InvalidIPAddress );
        assert_throws( IPv4Address( "192.168.0.1.2" ), InvalidIPAddress );
        assert_throws( IPv4Address( "192.168.0.256" ), InvalidIPAddress );
    }

    void
    test_construct_either_version()
    {
        assert_equals( toString( IPAddressVersion::IPv4 ), "4" );
        assert_equals( toString( IPAddressVersion::IPv6 ), "6" );
        {
            ::std::ostringstream oss;
            oss << IPAddressVersion::IPv4;
            assert_equals( oss.str(), "4" );
        }
        {
            ::std::ostringstream oss;
            oss << IPAddressVersion::IPv6;
            assert_equals( oss.str(), "6" );
        }

        auto pAddress( IPAddress::create( "43.201.17.9" ) );
        assert_not_that( !pAddress, "The address should not be null." );
        assert_equals( pAddress->version(), IPAddressVersion::IPv4, "The IP address should be IPv4." );
        assert_equals( pAddress->maximumPrefixLength(), 32, "The maximum prefix length should be 32." );
        assert_equals( toString( *pAddress ), "43.201.17.9" );

        pAddress = IPAddress::create( "2001::faf0:dead:beef:1" );
        assert_not_that( !pAddress, "The address should not be null." );
        assert_equals( pAddress->version(), IPAddressVersion::IPv6, "The IP address should be IPv6." );
        assert_equals( pAddress->maximumPrefixLength(), 128, "The maximum prefix length should be 128." );
        assert_equals( toString( *pAddress ), "2001::faf0:dead:beef:1" );

        assert_throws_message_contains(
            std::ignore = IPAddress::create( "192.168.0.1.2" ),
            InvalidIPAddress,
            "could not be converted to either an IPv4 or an IPv6 address" );
        assert_throws_message_contains(
            std::ignore = IPAddress::create( "2001::faf0:dead:beef::1" ),
            InvalidIPAddress,
            "could not be converted to either an IPv4 or an IPv6 address" );
    }

#ifdef ODDSOURCE_INCLUDE_BOOST
    void
    test_boost_address_conversion()
    {
        IPv4Address const address( "172.19.52.141" );
        auto const genericBoostAddress( static_cast< boost::asio::ip::address >( address ) );
        assert_that( genericBoostAddress.is_v4(), "The generic Boost address should be an IPv4 address." );
        assert_equals( genericBoostAddress.to_string(), "172.19.52.141" );

        auto const versionedBoostAddress( static_cast< boost::asio::ip::address_v4 >( address ) );
        assert_equals( versionedBoostAddress.to_string(), "172.19.52.141" );

        IPv4Address const genericRoundTrip( genericBoostAddress );
        assert_equals( toString( genericRoundTrip ), "172.19.52.141" );

        IPv4Address const versionedRoundTrip( versionedBoostAddress );
        assert_equals( toString( versionedRoundTrip ), "172.19.52.141" );

        auto const v6Address( boost::asio::ip::make_address( "2001:471:c2bd:bb61:6d7b:48a5:6304:31e5" ) );
        assert_throws( IPv4Address{ v6Address }, InvalidIPAddress );
    }
#endif

    [[maybe_unused]]
    static
    std::unique_ptr< Test >
    create()
    {
        return std::make_unique< TestIPv4Address >();
    }
};

namespace
{
    [[maybe_unused]]
    Tests::Test::Registrar< TestIPv4Address > registrar( "TestIPv4Address" );
}
