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

class TestIPv6Address : public Tests::Test
{
public:
    TestIPv6Address()
    {
        add_test( test_equals );
        add_test( test_string_round_trip_unscoped );
        add_test( test_string_round_trip_scoped );
        add_test( test_in_addr_round_trip_unscoped );
        add_test( test_in_addr_round_trip_scoped );
        add_test( test_bytes_round_trip_unscoped );
        add_test( test_bytes_round_trip_scoped );
        add_test( test_normalize );
        add_test( test_unspecified_address );
        add_test( test_loopback_address );
        add_test( test_link_local_addresses );
        add_test( test_site_local_addresses );
        add_test( test_unique_local_addresses );
        add_test( test_multicast_addresses );
        add_test( test_v4_mapped_addresses );
        add_test( test_v4_translated_addresses );
        add_test( test_v4_compatible_addresses );
        add_test( test_6to4_addresses );
        add_test( test_construct_malformed );

#ifdef ODDSOURCE_INCLUDE_BOOST
        add_test( test_boost_address_conversion );
#endif
    }

    void
    test_equals()
    {
        assert_equals( IPv6Address( "2001::dead:beef" ), IPv6Address( "2001::DEAD:BEEF" ) );
        assert_equals( IPv6Address( "2001::dead:beef" ), IPv6Address( "2001:0:0:0:0:0:dead:beef" ) );
        assert_not_equals( IPv6Address( "2001::dead:beef" ), IPv6Address( "2001::de:ad:be:ef" ) );
    }

    void
    test_string_round_trip_unscoped()
    {
        IPv6Address const address( "2001::dead:beef" );
        assert_equals( static_cast< ::std::string >( address ), "2001::dead:beef" );
        assert_equals(
                ::strcmp( static_cast< char const * >( address ), "2001::dead:beef" ),
                0,
                "The C strings do not match." );
        assert_not_that( address.has_scope_id() );
        assert_that( !address.scope_id(), "There should be no scope ID." );
        assert_that( !address.scope_name(), "There should be no scope name." );
        assert_equals( address.without_scope_id(), "2001::dead:beef" );

        ::std::ostringstream oss;
        oss << IPv6Address( "2001::de:ad:be:ef" );
        assert_equals( oss.str(), "2001::de:ad:be:ef" );
    }

    void
    test_string_round_trip_scoped()
    {
        IPv6Address const address( "fe80::f1:1612:447b:70c5%en0" );
        assert_equals( static_cast< ::std::string >( address ), "fe80::f1:1612:447b:70c5%en0" );
        assert_equals(
            ::strcmp( static_cast< char const * >( address ), "fe80::f1:1612:447b:70c5%en0" ),
            0,
            "The C strings do not match." );
        assert_that( address.has_scope_id() );
        assert_that( static_cast< bool >( address.scope_name() ) );
        assert_equals( *address.scope_name(), "en0" );
        assert_equals( *address.scope_name_or_id(), "en0" );
        assert_equals( address.without_scope_id(), "fe80::f1:1612:447b:70c5" );

        ::std::ostringstream oss;
        oss << IPv6Address( "fe80::b0fb:b8ff:fe5b:84e8%awl1" );
        assert_equals( oss.str(), "fe80::b0fb:b8ff:fe5b:84e8%awl1" );

        IPv6Address const address2( "fe80::f1:1612:447b:70c5%117" );
        assert_equals( static_cast< ::std::string >( address2 ), "fe80::f1:1612:447b:70c5%117" );
        assert_equals(
            ::strcmp( static_cast< char const * >( address2 ), "fe80::f1:1612:447b:70c5%117" ),
            0,
            "The C strings do not match." );
        assert_that( address2.has_scope_id() );
        assert_that( static_cast< bool >( address2.scope_id() ) );
        assert_equals( *address2.scope_id(), 117u );
        assert_equals( *address2.scope_name_or_id(), "117" );
        assert_equals( *address2.scope_id_or_name(), "117" );
        assert_equals( address2.without_scope_id(), "fe80::f1:1612:447b:70c5" );
    }

    void
    test_in_addr_round_trip_unscoped()
    {
        using namespace ::std::string_literals;
        in6_addr data {};
        inet_pton( AF_INET6, "2001:471:c2bd:bb61:6d7b:48a5:6304:31e5", &data );
        IPv6Address const address( &data );
        assert_equals( static_cast< ::std::string >( address ), "2001:471:c2bd:bb61:6d7b:48a5:6304:31e5" );
        assert_not_that( address.has_scope_id() );

        assert_equals( address.version(), IPAddressVersion::IPv6 );
        assert_equals( address.maximum_prefix_length(), 128 );

        auto owned( static_cast< in6_addr const * >( address ) );
        auto bytes1( reinterpret_cast< uint8_t const * >( owned ) );
        auto bytes2( reinterpret_cast< uint8_t const * >( &data ) );

        for( size_t i{ 0 }; i < sizeof( in6_addr ); i++ )
        {
            assert_equals( bytes1[ i ], bytes2[ i ], "Bytes "s + ::std::to_string( i ) + " do not match."s );
        }
    }

    void
    test_in_addr_round_trip_scoped()
    {
        using namespace std::string_literals;
        in6_addr data {};
        inet_pton( AF_INET6, "fe80::f1:1612:447b:70c5", &data );
        IPv6Address const address( &data, 117 );
        assert_equals( static_cast< ::std::string >( address ), "fe80::f1:1612:447b:70c5%117" );
        assert_that( address.has_scope_id() );
        assert_that( static_cast< bool >( address.scope_id() ) );
        assert_equals( *address.scope_id(), 117u );
        assert_equals( address.without_scope_id(), "fe80::f1:1612:447b:70c5" );

        assert_equals( address.version(), IPAddressVersion::IPv6 );
        assert_equals( address.maximum_prefix_length(), 128 );

        auto owned( static_cast< in6_addr const * >( address ) );
        auto bytes1( reinterpret_cast< uint8_t const * >( owned ) );
        auto bytes2( reinterpret_cast< uint8_t const * >( &data ) );

        for( size_t i{ 0 }; i < sizeof( in6_addr ); i++ )
        {
            assert_equals( bytes1[ i ], bytes2[ i ], "Bytes "s + ::std::to_string( i ) + " do not match."s );
        }
    }

    void
    test_bytes_round_trip_unscoped()
    {
        using namespace ::std::string_literals;
        IPv6Address::Bytes const data{
            0x20, 0x01,
            0x04, 0x71,
            0xc2, 0xbd,
            0xbb, 0x61,
            0x06, 0x7b,
            0x48, 0xa5,
            0x63, 0x04,
            0x31, 0x5e };
        IPv6Address const address( data );
        assert_equals( static_cast< ::std::string >( address ), "2001:471:c2bd:bb61:67b:48a5:6304:315e" );
        assert_not_that( address.has_scope_id() );

        assert_equals( address.version(), IPAddressVersion::IPv6 );
        assert_equals( address.maximum_prefix_length(), 128 );

        auto const bytes( static_cast< IPv6Address::Bytes >( address ) );
        for( size_t i{ 0 }; i < 16; ++i )
        {
            assert_equals( bytes[ i ], data[ i ], "Bytes "s + ::std::to_string( i ) + " do not match."s );
        }
    }

    void
    test_bytes_round_trip_scoped()
    {
        using namespace std::string_literals;
        IPv6Address::Bytes const data{
            0xfe, 0x80,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x00,
            0x00, 0x1f,
            0x12, 0x16,
            0x7b, 0x44,
            0xc5, 0x70 };
        IPv6Address const address( data, 32 );
        assert_equals( static_cast< ::std::string >( address ), "fe80::1f:1216:7b44:c570%32" );
        assert_that( address.has_scope_id() );
        assert_that( static_cast< bool >( address.scope_id() ) );
        assert_equals( *address.scope_id(), 32u );
        assert_equals( address.without_scope_id(), "fe80::1f:1216:7b44:c570" );

        assert_equals( address.version(), IPAddressVersion::IPv6 );
        assert_equals( address.maximum_prefix_length(), 128 );

        auto const bytes( static_cast< IPv6Address::Bytes >( address ) );
        for( size_t i{ 0 }; i < 16; ++i )
        {
            assert_equals( bytes[ i ], data[ i ], "Bytes "s + ::std::to_string( i ) + " do not match."s );
        }
    }

    void
    test_normalize()
    {
        IPv6Address const address( "2001:0:0:0:de:AD:be:EF" );
        assert_equals( static_cast< ::std::string >( address ), "2001:0:0:0:de:AD:be:EF" );
        assert_equals( static_cast< ::std::string >( address.normalize() ), "2001::de:ad:be:ef" );
    }

    void
    test_unspecified_address()
    {
        IPv6Address const address( "::" );
        assert_that( address.is_unspecified(), ":: should be unspecified." );
        assert_that( address.is_reserved(), ":: should be reserved." );
        assert_that( !address.is_private(), ":: should not be private." );
        assert_that( !address.is_loopback(), ":: should not be the loopback." );
        assert_that( !address.is_link_local(), ":: should not be link-local." );
        assert_that( !address.is_site_local(), ":: should not be site-local." );
        assert_that( !address.is_unique_local(), ":: should not be unique-local." );
        assert_that( !address.is_multicast(), ":: should not be multicast." );
        assert_that( !address.is_v4_mapped(), ":: should not be v4-mapped." );
        assert_that( !address.is_v4_translated(), ":: should not be v4-translated." );
        assert_that( !address.is_v4_compatible(), ":: should not be v4-compatible." );
        assert_that( !address.is_6to4(), ":: should not be 6to4." );

        assert_that( !address.multicast_scope(), ":: should not have a multicast scope." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
            ":: should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
            ":: should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
            ":: should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
            ":: should have no multicast flags." );

        assert_that( !IPv6Address( "::1" ).is_unspecified(), "::1 should not be unspecified." );
    }

    void
    test_loopback_address()
    {
        IPv6Address const address( "::1" );
        assert_that( address.is_loopback(), "::1 should be the loopback." );
        assert_that( address.is_reserved(), "::1 should be reserved." );
        assert_that( !address.is_private(), "::1 should not be private." );
        assert_that( !address.is_unspecified(), "::1 should not be unspecified." );
        assert_that( !address.is_link_local(), "::1 should not be link-local." );
        assert_that( !address.is_site_local(), "::1 should not be site-local." );
        assert_that( !address.is_unique_local(), "::1 should not be unique-local." );
        assert_that( !address.is_multicast(), "::1 should not be multicast." );
        assert_that( !address.is_v4_mapped(), "::1 should not be v4-mapped." );
        assert_that( !address.is_v4_translated(), "::1 should not be v4-translated." );
        assert_that( !address.is_v4_compatible(), "::1 should not be v4-compatible." );
        assert_that( !address.is_6to4(), "::1 should not be 6to4." );

        assert_that( !address.multicast_scope(), "::1 should not have a multicast scope." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
            "::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
            "::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
            "::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
            "::1 should have no multicast flags." );

        assert_that( !IPv6Address( "::2" ).is_loopback(), "::2 should not be the loopback." );
    }

    void
    test_link_local_addresses()
    {
        IPv6Address const address( "fe80::1" );
        assert_that( address.is_link_local(), "fe80::1 should be link-local." );
        assert_that( address.is_reserved(), "fe80::1 should be reserved." );
        assert_that( !address.is_private(), "fe80::1 should not be private." );
        assert_that( !address.is_loopback(), "fe80::1 should not be the loopback." );
        assert_that( !address.is_unspecified(), "fe80::1 should not be unspecified." );
        assert_that( !address.is_site_local(), "fe80::1 should not be site-local." );
        assert_that( !address.is_unique_local(), "fe80::1 should not be unique-local." );
        assert_that( !address.is_multicast(), "fe80::1 should not be multicast." );
        assert_that( !address.is_v4_mapped(), "fe80::1 should not be v4-mapped." );
        assert_that( !address.is_v4_translated(), "fe80::1 should not be v4-translated." );
        assert_that( !address.is_v4_compatible(), "fe80::1 should not be v4-compatible." );
        assert_that( !address.is_6to4(), "fe80::1 should not be 6to4." );

        assert_that( !address.multicast_scope(), "fe80::1 should not have a multicast scope." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
            "fe80::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
            "fe80::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
            "fe80::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
            "fe80::1 should have no multicast flags." );

        assert_that(
            !IPv6Address( "fe79:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ).is_link_local(),
            "fe79:ffff:ffff:ffff:ffff:ffff:ffff:ffff should not be link-local." );
        assert_that( IPv6Address( "fe80::2" ).is_link_local(), "fe80::2 should be link-local." );
        assert_that(
            IPv6Address( "fe80::ffff:ffff:ffff:ffff" ).is_link_local(),
            "fe80::ffff:ffff:ffff:ffff should be link-local." );
        assert_that( !IPv6Address( "fe80::1:0:0:0:0" ).is_link_local(), "fe80::1:0:0:0:0 should not be link-local." );
    }

    void
    test_site_local_addresses()
    {
        IPv6Address const address( "fec0::1" );
        assert_that( address.is_site_local(), "fec0::1 should be site-local." );
        assert_that( address.is_reserved(), "fec0::1 should be reserved." );
        assert_that( address.is_private(), "fec0::1 should be private." );
        assert_that( !address.is_loopback(), "fec0::1 should not be the loopback." );
        assert_that( !address.is_link_local(), "fec0::1 should not be link-local." );
        assert_that( !address.is_unspecified(), "fec0::1 should not be unspecified." );
        assert_that( !address.is_unique_local(), "fec0::1 should not be unique-local." );
        assert_that( !address.is_multicast(), "fec0::1 should not be multicast." );
        assert_that( !address.is_v4_mapped(), "fec0::1 should not be v4-mapped." );
        assert_that( !address.is_v4_translated(), "fec0::1 should not be v4-translated." );
        assert_that( !address.is_v4_compatible(), "fec0::1 should not be v4-compatible." );
        assert_that( !address.is_6to4(), "fec0::1 should not be 6to4." );

        assert_that( !address.multicast_scope(), "fec0::1 should not have a multicast scope." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
            "fec0::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
            "fec0::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
            "fec0::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
            "fec0::1 should have no multicast flags." );

        assert_that(
            !IPv6Address( "febf:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ).is_site_local(),
            "febf:ffff:ffff:ffff:ffff:ffff:ffff:ffff should not be site-local." );
        assert_that( IPv6Address( "fec0::2" ).is_site_local(), "fec0::2 should be site-local." );
        assert_that(
            IPv6Address( "fec0:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ).is_site_local(),
            "fec0:ffff:ffff:ffff:ffff:ffff:ffff:ffff should be site-local." );
        assert_that(
            IPv6Address( "feff:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ).is_site_local(),
            "feff:ffff:ffff:ffff:ffff:ffff:ffff:ffff should be site-local." );
        assert_that( !IPv6Address( "ff00::1" ).is_site_local(), "ff00::1 should not be site-local." );
    }

    void
    test_unique_local_addresses()
    {
        IPv6Address const address( "fc00::1" );
        assert_that( address.is_unique_local(), "fc00::1 should be unique_local." );
        assert_that( address.is_reserved(), "fc00::1 should be reserved." );
        assert_that( address.is_private(), "fc00::1 should be private." );
        assert_that( !address.is_loopback(), "fc00::1 should not be the loopback." );
        assert_that( !address.is_link_local(), "fc00::1 should not be link-local." );
        assert_that( !address.is_site_local(), "fc00::1 should not be site-local." );
        assert_that( !address.is_unspecified(), "fc00::1 should not be unspecified." );
        assert_that( !address.is_multicast(), "fc00::1 should not be multicast." );
        assert_that( !address.is_v4_mapped(), "fc00::1 should not be v4-mapped." );
        assert_that( !address.is_v4_translated(), "fc00::1 should not be v4-translated." );
        assert_that( !address.is_v4_compatible(), "fc00::1 should not be v4-compatible." );
        assert_that( !address.is_6to4(), "fc00::1 should not be 6to4." );

        assert_that( !address.multicast_scope(), "fc00::1 should not have a multicast scope." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
            "fc00::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
            "fc00::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
            "fc00::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
            "fc00::1 should have no multicast flags." );

        assert_that(
            !IPv6Address( "fbff:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ).is_unique_local(),
            "fbff:ffff:ffff:ffff:ffff:ffff:ffff:ffff should not be unique local." );
        assert_that( IPv6Address( "fc00::2" ).is_unique_local(), "fc00::2 should be unique local." );
        assert_that(
            IPv6Address( "fc00:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ).is_unique_local(),
            "fc00:ffff:ffff:ffff:ffff:ffff:ffff:ffff should be unique local." );
        assert_that(
            IPv6Address( "FCFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF" ).is_unique_local(),
            "fcff:ffff:ffff:ffff:ffff:ffff:ffff:ffff should be unique local." );
        assert_that(
            IPv6Address( "FDFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF" ).is_unique_local(),
            "fdff:ffff:ffff:ffff:ffff:ffff:ffff:ffff should be unique local." );
        assert_that( !IPv6Address( "fe00::1" ).is_unique_local(), "fe00::1 should not be unique local." );
    }

    void
    test_multicast_addresses()
    {
        IPv6Address const address( "ff00::1" );
        assert_that( address.is_multicast(), "ff00::1 should be multicast." );
        assert_that( address.is_reserved(), "ff00::1 should be reserved." );
        assert_that( !address.is_private(), "ff00::1 should not be private." );
        assert_that( !address.is_loopback(), "ff00::1 should not be the loopback." );
        assert_that( !address.is_link_local(), "ff00::1 should not be link-local." );
        assert_that( !address.is_site_local(), "ff00::1 should not be site-local." );
        assert_that( !address.is_unique_local(), "ff00::1 should not be unique-local." );
        assert_that( !address.is_unspecified(), "ff00::1 should not be unspecified." );
        assert_that( !address.is_v4_mapped(), "ff00::1 should not be v4-mapped." );
        assert_that( !address.is_v4_translated(), "ff00::1 should not be v4-translated." );
        assert_that( !address.is_v4_compatible(), "ff00::1 should not be v4-compatible." );
        assert_that( !address.is_6to4(), "ff00::1 should not be 6to4." );

        assert_equals( *address.multicast_scope(), MulticastScope::Reserved );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
            ":: should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
            ":: should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
            ":: should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
            ":: should have no multicast flags." );

        assert_that(
            !IPv6Address( "feff:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ).is_multicast(),
            "feff:ffff:ffff:ffff:ffff:ffff:ffff:ffff should not be multicast." );
        assert_that( IPv6Address( "ff00::2" ).is_multicast(), "ff00::2 should be multicast." );
        assert_that(
            IPv6Address( "ff00:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ).is_multicast(),
            "ff00:ffff:ffff:ffff:ffff:ffff:ffff:ffff should be multicast." );
        assert_that(
            IPv6Address( "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ).is_multicast(),
            "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff should be multicast." );

        assert_equals( *IPv6Address( "ff01::1" ).multicast_scope(), MulticastScope::InterfaceLocal );
        assert_equals( *IPv6Address( "ff02::1" ).multicast_scope(), MulticastScope::LinkLocal );
        assert_equals( *IPv6Address( "ff03::1" ).multicast_scope(), MulticastScope::RealmLocal );
        assert_equals( *IPv6Address( "ff04::1" ).multicast_scope(), MulticastScope::AdminLocal );
        assert_equals( *IPv6Address( "ff05::1" ).multicast_scope(), MulticastScope::SiteLocal );
        assert_equals( *IPv6Address( "ff06::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff07::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff08::1" ).multicast_scope(), MulticastScope::OrganizationLocal );
        assert_equals( *IPv6Address( "ff09::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff0a::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff0b::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff0c::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff0d::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff0e::1" ).multicast_scope(), MulticastScope::Global );
        assert_equals( *IPv6Address( "ff0f::1" ).multicast_scope(), MulticastScope::Reserved );

        assert_equals( *IPv6Address( "ff11::1" ).multicast_scope(), MulticastScope::InterfaceLocal );
        assert_equals( *IPv6Address( "ff12::1" ).multicast_scope(), MulticastScope::LinkLocal );
        assert_equals( *IPv6Address( "ff13::1" ).multicast_scope(), MulticastScope::RealmLocal );
        assert_equals( *IPv6Address( "ff14::1" ).multicast_scope(), MulticastScope::AdminLocal );
        assert_equals( *IPv6Address( "ff15::1" ).multicast_scope(), MulticastScope::SiteLocal );
        assert_equals( *IPv6Address( "ff16::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff17::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff18::1" ).multicast_scope(), MulticastScope::OrganizationLocal );
        assert_equals( *IPv6Address( "ff19::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff1a::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff1b::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff1c::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff1d::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff1e::1" ).multicast_scope(), MulticastScope::Global );
        assert_equals( *IPv6Address( "ff1f::1" ).multicast_scope(), MulticastScope::Reserved );

        assert_equals( *IPv6Address( "ff21::1" ).multicast_scope(), MulticastScope::InterfaceLocal );
        assert_equals( *IPv6Address( "ff22::1" ).multicast_scope(), MulticastScope::LinkLocal );
        assert_equals( *IPv6Address( "ff23::1" ).multicast_scope(), MulticastScope::RealmLocal );
        assert_equals( *IPv6Address( "ff24::1" ).multicast_scope(), MulticastScope::AdminLocal );
        assert_equals( *IPv6Address( "ff25::1" ).multicast_scope(), MulticastScope::SiteLocal );
        assert_equals( *IPv6Address( "ff26::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff27::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff28::1" ).multicast_scope(), MulticastScope::OrganizationLocal );
        assert_equals( *IPv6Address( "ff29::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff2a::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff2b::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff2c::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff2d::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff2e::1" ).multicast_scope(), MulticastScope::Global );
        assert_equals( *IPv6Address( "ff2f::1" ).multicast_scope(), MulticastScope::Reserved );

        assert_equals( *IPv6Address( "ff71::1" ).multicast_scope(), MulticastScope::InterfaceLocal );
        assert_equals( *IPv6Address( "ff72::1" ).multicast_scope(), MulticastScope::LinkLocal );
        assert_equals( *IPv6Address( "ff73::1" ).multicast_scope(), MulticastScope::RealmLocal );
        assert_equals( *IPv6Address( "ff74::1" ).multicast_scope(), MulticastScope::AdminLocal );
        assert_equals( *IPv6Address( "ff75::1" ).multicast_scope(), MulticastScope::SiteLocal );
        assert_equals( *IPv6Address( "ff76::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff77::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff78::1" ).multicast_scope(), MulticastScope::OrganizationLocal );
        assert_equals( *IPv6Address( "ff79::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff7a::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff7b::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff7c::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff7d::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "ff7e::1" ).multicast_scope(), MulticastScope::Global );
        assert_equals( *IPv6Address( "ff7f::1" ).multicast_scope(), MulticastScope::Reserved );

        assert_equals( *IPv6Address( "fff1::1" ).multicast_scope(), MulticastScope::InterfaceLocal );
        assert_equals( *IPv6Address( "fff2::1" ).multicast_scope(), MulticastScope::LinkLocal );
        assert_equals( *IPv6Address( "fff3::1" ).multicast_scope(), MulticastScope::RealmLocal );
        assert_equals( *IPv6Address( "fff4::1" ).multicast_scope(), MulticastScope::AdminLocal );
        assert_equals( *IPv6Address( "fff5::1" ).multicast_scope(), MulticastScope::SiteLocal );
        assert_equals( *IPv6Address( "fff6::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "fff7::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "fff8::1" ).multicast_scope(), MulticastScope::OrganizationLocal );
        assert_equals( *IPv6Address( "fff9::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "fffa::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "fffb::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "fffc::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "fffd::1" ).multicast_scope(), MulticastScope::Unassigned );
        assert_equals( *IPv6Address( "fffe::1" ).multicast_scope(), MulticastScope::Global );
        assert_equals( *IPv6Address( "ffff::1" ).multicast_scope(), MulticastScope::Reserved );

        {
            IPv6Address const a( "ff10::1" );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
                ":: should have DynamicallyAssigned flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
                ":: should not have PrefixBased flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
                ":: should not have RendezvousEmbedded flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
                ":: should not have Reserved flag." );
        }

        {
            IPv6Address const a( "ff20::1" );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
                ":: should not have DynamicallyAssigned flag." );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
                ":: should have PrefixBased flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
                ":: should not have RendezvousEmbedded flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
                ":: should not have Reserved flag." );
        }

        {
            IPv6Address const a( "ff40::1" );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
                ":: should not have DynamicallyAssigned flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
                ":: should not have PrefixBased flag." );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
                ":: should have RendezvousEmbedded flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
                ":: should not have Reserved flag." );
        }

        {
            IPv6Address const a( "ff80::1" );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
                ":: should not have DynamicallyAssigned flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
                ":: should not have PrefixBased flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
                ":: should not have RendezvousEmbedded flag." );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
                ":: should have Reserved flag." );
        }

        {
            IPv6Address const a( "ff30::1" );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
                ":: should have DynamicallyAssigned flag." );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
                ":: should have PrefixBased flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
                ":: should not have RendezvousEmbedded flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
                ":: should not have Reserved flag." );
        }

        {
            IPv6Address const a( "ff60::1" );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
                ":: should not have DynamicallyAssigned flag." );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
                ":: should have PrefixBased flag." );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
                ":: should have RendezvousEmbedded flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
                ":: should not have Reserved flag." );
        }

        {
            IPv6Address const a( "ff70::1" );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
                ":: should have DynamicallyAssigned flag." );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
                ":: should have PrefixBased flag." );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
                ":: should have RendezvousEmbedded flag." );
            assert_that(
                !a.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
                ":: should not have Reserved flag." );
        }

        {
            IPv6Address const a( "fff0::1" );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
                ":: should have DynamicallyAssigned flag." );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
                ":: should have PrefixBased flag." );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
                ":: should have RendezvousEmbedded flag." );
            assert_that(
                a.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
                ":: should have Reserved flag." );
        }
    }

    void
    test_v4_mapped_addresses()
    {
        IPv6Address const address( "::ffff:0:1" );
        assert_that( address.is_v4_mapped(), "::ffff:0:1 should be v4-mapped." );
        assert_that( address.is_reserved(), "::ffff:0:1 should be reserved." );
        assert_that( !address.is_private(), "::ffff:0:1 should not be private." );
        assert_that( !address.is_loopback(), "::ffff:0:1 should not be the loopback." );
        assert_that( !address.is_link_local(), "::ffff:0:1 should not be link-local." );
        assert_that( !address.is_site_local(), "::ffff:0:1 should not be site-local." );
        assert_that( !address.is_unique_local(), "::ffff:0:1 should not be unique-local." );
        assert_that( !address.is_multicast(), "::ffff:0:1 should not be multicast." );
        assert_that( !address.is_unspecified(), "::ffff:0:1 should not be unspecified." );
        assert_that( !address.is_v4_translated(), "::ffff:0:1 should not be v4-translated." );
        assert_that( !address.is_v4_compatible(), "::ffff:0:1 should not be v4-compatible." );
        assert_that( !address.is_6to4(), "::ffff:0:1 should not be 6to4." );

        assert_that( !address.multicast_scope(), "::ffff:0:1 should not have a multicast scope." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
            "::ffff:0:1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
            "::ffff:0:1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
            "::ffff:0:1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
            "::ffff:0:1 should have no multicast flags." );

        assert_that(
            !IPv6Address( "::fffe:ffff:ffff" ).is_v4_mapped(),
            "::fffe:ffff:ffff should not be v4-mapped." );
        assert_that(
            IPv6Address( "::ffff:ffff:ffff" ).is_v4_mapped(),
            "::ffff:ffff:ffff should be v4-mapped." );
        assert_that(
            !IPv6Address( "::1:0:0:0" ).is_v4_mapped(),
            "::1:0:0:0 should not be v4-mapped." );

        {
            IPv6Address const a( "::ffff:127.0.0.1" );
            assert_that( a.is_v4_mapped(), "::ffff:127.0.0.1 should be v4-mapped" );
            assert_equals( static_cast< ::std::string >( a ), "::ffff:127.0.0.1" );
            assert_equals( static_cast< ::std::string >( a.normalize() ), "::ffff:127.0.0.1" );
        }

        {
            IPv6Address const a( "::ffff:201.53.78.3" );
            assert_that( a.is_v4_mapped(), "::ffff:201.53.78.3 should be v4-mapped" );
            assert_equals( static_cast< ::std::string >( a ), "::ffff:201.53.78.3" );
            assert_equals( static_cast< ::std::string >( a.normalize() ), "::ffff:201.53.78.3" );
        }
    }

    void
    test_v4_translated_addresses()
    {
        IPv6Address const address( "::ffff:0:0:1" );
        assert_that( address.is_v4_translated(), "::ffff:0:0:1 should be v4-translated." );
        assert_that( address.is_reserved(), "::ffff:0:0:1 should be reserved." );
        assert_that( !address.is_private(), "::ffff:0:0:1 should not be private." );
        assert_that( !address.is_loopback(), "::ffff:0:0:1 should not be the loopback." );
        assert_that( !address.is_link_local(), "::ffff:0:0:1 should not be link-local." );
        assert_that( !address.is_site_local(), "::ffff:0:0:1 should not be site-local." );
        assert_that( !address.is_unique_local(), "::ffff:0:0:1 should not be unique-local." );
        assert_that( !address.is_multicast(), "::ffff:0:0:1 should not be multicast." );
        assert_that( !address.is_v4_mapped(), "::ffff:0:0:1 should not be v4-mapped." );
        assert_that( !address.is_unspecified(), "::ffff:0:0:1 should not be unspecified." );
        assert_that( !address.is_v4_compatible(), "::ffff:0:0:1 should not be v4-compatible." );
        assert_that( !address.is_6to4(), "::ffff:0:0:1 should not be 6to4." );

        assert_that( !address.multicast_scope(), "::ffff:0:0:1 should not have a multicast scope." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
            "::ffff:0:0:1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
            "::ffff:0:0:1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
            "::ffff:0:0:1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
            "::ffff:0:0:1 should have no multicast flags." );

        assert_that(
            !IPv6Address( "::fffe:0:ffff:ffff" ).is_v4_translated(),
            "::fffe:0:ffff:ffff should not be v4-translated." );
        assert_that(
            IPv6Address( "::ffff:0:ffff:ffff" ).is_v4_translated(),
            "::ffff:0:ffff:ffff should be v4-translated." );
        assert_that(
            !IPv6Address( "::ffff:1:0:0" ).is_v4_translated(),
            "::ffff:1:0:0 should not be v4-translated." );

        {
            IPv6Address const a( "::ffff:0:127.0.0.1" );
            assert_that( a.is_v4_translated(), "::ffff:0:127.0.0.1 should be v4-translated" );
            assert_equals( static_cast< ::std::string >( a ), "::ffff:0:127.0.0.1" );
#ifdef ODDSOURCE_IS_WINDOWS
            assert_equals( static_cast< ::std::string >( a.normalize() ), "::ffff:0:127.0.0.1" );
#else /* ODDSOURCE_IS_WINDOWS */
            assert_equals( static_cast< ::std::string >( a.normalize() ), "::ffff:0:7f00:1" );
#endif /* !ODDSOURCE_IS_WINDOWS */
        }

        {
            IPv6Address const a( "::ffff:0:201.53.78.3" );
            assert_that( a.is_v4_translated(), "::ffff:0:201.53.78.3 should be v4-translated" );
            assert_equals( static_cast< ::std::string >( a ), "::ffff:0:201.53.78.3" );
#ifdef ODDSOURCE_IS_WINDOWS
            assert_equals( static_cast< ::std::string >( a.normalize() ), "::ffff:0:201.53.78.3" );
#else /* ODDSOURCE_IS_WINDOWS */
            assert_equals( static_cast< ::std::string >( a.normalize() ), "::ffff:0:c935:4e03" );
#endif /* !ODDSOURCE_IS_WINDOWS */
        }
    }

    void
    test_v4_compatible_addresses()
    {
        IPv6Address const address( "::0.0.0.2" );
        assert_that( address.is_v4_compatible(), "::0.0.0.2 should be v4-compatible." );
        assert_that( address.is_reserved(), "::0.0.0.2 should be reserved." );
        assert_that( !address.is_private(), "::0.0.0.2 should not be private." );
        assert_that( !address.is_loopback(), "::0.0.0.2 should not be the loopback." );
        assert_that( !address.is_link_local(), "::0.0.0.2 should not be link-local." );
        assert_that( !address.is_site_local(), "::0.0.0.2 should not be site-local." );
        assert_that( !address.is_unique_local(), "::0.0.0.2 should not be unique-local." );
        assert_that( !address.is_multicast(), "::0.0.0.2 should not be multicast." );
        assert_that( !address.is_v4_mapped(), "::0.0.0.2 should not be v4-mapped." );
        assert_that( !address.is_v4_translated(), "::0.0.0.2 should not be v4-translated." );
        assert_that( !address.is_unspecified(), "::0.0.0.2 should not be unspecified." );
        assert_that( !address.is_6to4(), "::0.0.0.2 should not be 6to4." );

        assert_that( !address.multicast_scope(), "::0.0.0.2 should not have a multicast scope." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
            "::0.0.0.2 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
            "::0.0.0.2 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
            "::0.0.0.2 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
            "::0.0.0.2 should have no multicast flags." );

        assert_that(
            !IPv6Address( "::1" ).is_v4_compatible(),
            "::1 should not be v4-compatible." );
        assert_that(
            IPv6Address( "::ffff:ffff" ).is_v4_compatible(),
            "::ffff:ffff should be v4-compatible." );
        assert_that(
            !IPv6Address( "::1:0:0" ).is_v4_compatible(),
            "::1:0:0 should not be v4-compatible." );

        {
            IPv6Address const a( "::127.0.0.1" );
            assert_that( a.is_v4_compatible(), "::127.0.0.1 should be v4-compatible" );
            assert_equals( static_cast< ::std::string >( a ), "::127.0.0.1" );
            assert_equals( static_cast< ::std::string >( a.normalize() ), "::127.0.0.1" );
        }

        {
            IPv6Address const a( "::201.53.78.3" );
            assert_that( a.is_v4_compatible(), "::201.53.78.3 should be v4-compatible" );
            assert_equals( static_cast< ::std::string >( a ), "::201.53.78.3" );
            assert_equals( static_cast< ::std::string >( a.normalize() ), "::201.53.78.3" );
        }
    }

    void
    test_6to4_addresses()
    {
        IPv6Address const address( "2002::1" );
        assert_that( address.is_6to4(), "2002::1 should be 6to4." );
        assert_that( address.is_reserved(), "2002::1 should be reserved." );
        assert_that( !address.is_private(), "2002::1 should not be private." );
        assert_that( !address.is_loopback(), "2002::1 should not be the loopback." );
        assert_that( !address.is_link_local(), "2002::1 should not be link-local." );
        assert_that( !address.is_site_local(), "2002::1 should not be site-local." );
        assert_that( !address.is_unique_local(), "2002::1 should not be unique-local." );
        assert_that( !address.is_multicast(), "2002::1 should not be multicast." );
        assert_that( !address.is_v4_mapped(), "2002::1 should not be v4-mapped." );
        assert_that( !address.is_v4_translated(), "2002::1 should not be v4-translated." );
        assert_that( !address.is_v4_compatible(), "2002::1 should not be v4-compatible." );
        assert_that( !address.is_unspecified(), "2002::1 should not be unspecified." );

        assert_that(
            !address.multicast_scope(), "2002::1 should not have a multicast scope." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::DynamicallyAssigned ),
            "2002::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::PrefixBased ),
            "2002::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::RendezvousEmbedded ),
            "2002::1 should have no multicast flags." );
        assert_that(
            !address.is_multicast_flag_enabled( MulticastV6Flag::ReservedFlag ),
            "2002::1 should have no multicast flags." );

        assert_that(
            !IPv6Address( "2001:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ).is_6to4(),
            "2001:ffff:ffff:ffff:ffff:ffff:ffff:ffff should not be 6to4." );
        assert_that(
            IPv6Address( "2002:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ).is_6to4(),
            "2002:ffff:ffff:ffff:ffff:ffff:ffff:ffff should be 6to4." );
        assert_that(
            !IPv6Address( "2003::1" ).is_6to4(),
            "2003::1 should not be 6to4." );
    }

    void
    test_construct_malformed()
    {
        assert_throws( IPv6Address( "" ), InvalidIPAddress );
        assert_throws( IPv6Address( "ffff:ffff:ffff:ffff:ffff:ffff:ffff" ), InvalidIPAddress );
        assert_throws( IPv6Address( "ffff:ffff:ffff:ffff:ffff:ffff:ffff:fffg" ), InvalidIPAddress );
        assert_throws( IPv6Address( "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff:" ), InvalidIPAddress );
        assert_throws( IPv6Address( "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff" ), InvalidIPAddress );
        assert_throws( IPv6Address( "ffff::1::2" ), InvalidIPAddress );
    }

#ifdef ODDSOURCE_INCLUDE_BOOST
    void
    test_boost_address_conversion()
    {
        IPv6Address const address( "2001:471:c2bd:bb61:6d7b:48a5:6304:31e5" );
        auto const genericBoostAddress( static_cast< boost::asio::ip::address >( address ) );
        assert_that( genericBoostAddress.is_v6(), "The generic Boost address should be an IPv6 address." );
        assert_equals( genericBoostAddress.to_string(), "2001:471:c2bd:bb61:6d7b:48a5:6304:31e5" );

        auto const versionedBoostAddress( static_cast< boost::asio::ip::address_v6 >( address ) );
        assert_equals( versionedBoostAddress.to_string(), "2001:471:c2bd:bb61:6d7b:48a5:6304:31e5" );

        IPv6Address const genericRoundTrip( genericBoostAddress );
        assert_equals( toString( genericRoundTrip ), "2001:471:c2bd:bb61:6d7b:48a5:6304:31e5" );

        IPv6Address const versionedRoundTrip( versionedBoostAddress );
        assert_equals( toString( versionedRoundTrip ), "2001:471:c2bd:bb61:6d7b:48a5:6304:31e5" );

        auto const v4Address( boost::asio::ip::make_address( "129.173.55.4" ) );
        assert_throws( IPv6Address{ v4Address }, InvalidIPAddress );
    }
#endif

    [[maybe_unused]]
    static
    std::unique_ptr< Test >
    create()
    {
        return std::make_unique< TestIPv6Address >();
    }
};

namespace
{
    [[maybe_unused]]
    Tests::Test::Registrar<TestIPv6Address> registrar( "TestIPv6Address" );
}
