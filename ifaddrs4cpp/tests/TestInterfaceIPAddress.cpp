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

#include <oddsource/network/interfaces/Interface.hpp>
#include "main.h"

using namespace OddSource::Interfaces;

class TestInterfaceIPAddress : public Tests::Test
{
public:
    TestInterfaceIPAddress()
    {
        add_test( test_simple_v4 );
        add_test( test_v4_with_broadcast );
        add_test( test_v4_with_p2p );
        add_test( test_simple_v6 );
    }

    void
    test_simple_v4()
    {
        InterfaceIPAddress const address( IPv4Address( "201.17.159.33" ), 0, 24 );
        assert_equals( address.address(), IPv4Address( "201.17.159.33" ) );
        assert_equals( *address.prefix_length(), 24 );
        assert_not_that(
            static_cast< bool >( address.broadcast_address() ),
            "There should be no broadcast address." );
        assert_not_that(
            static_cast< bool >( address.point_to_point_destination() ),
            "There should be no P2P destination." );

        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Anycast ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::AutoConfigured ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Deprecated ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Detached ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Duplicated ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Dynamic ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::NoDad ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Optimistic ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Secured ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Temporary ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Tentative ) );
    }

    void
    test_v4_with_broadcast()
    {
        InterfaceIPAddress const address(
                IPv4Address( "209.53.101.102" ),
                InterfaceIPAddressFlag::Dynamic | InterfaceIPAddressFlag::Secured,
                16,
                Broadcast,
                IPv4Address( "209.53.255.254" ) );
        assert_equals( address.address(), IPv4Address( "209.53.101.102" ) );
        assert_equals( *address.prefix_length(), 16 );
        assert_equals( *address.broadcast_address(), IPv4Address( "209.53.255.254" ) );
        assert_not_that(
            static_cast< bool >( address.point_to_point_destination() ),
            "There should be no P2P destination." );

        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Anycast ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::AutoConfigured ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Deprecated ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Detached ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Duplicated ) );
        assert_that( address.is_flag_enabled( InterfaceIPAddressFlag::Dynamic ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::NoDad ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Optimistic ) );
        assert_that( address.is_flag_enabled( InterfaceIPAddressFlag::Secured ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Temporary ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Tentative ) );
    }

    void
    test_v4_with_p2p()
    {
        InterfaceIPAddress const address(
                IPv4Address( "209.53.101.102" ),
                InterfaceIPAddressFlag::AutoConfigured | InterfaceIPAddressFlag::NoDad |
                    InterfaceIPAddressFlag::Optimistic,
                0,
                PointToPoint,
                IPv4Address( "209.53.255.1" ) );
        assert_equals( address.address(), IPv4Address( "209.53.101.102" ) );
        assert_not_that(
            static_cast< bool >( address.prefix_length() ),
            "There should be no prefix length." );
        assert_not_that(
            static_cast< bool >( address.broadcast_address() ),
            "There should be no broadcast address." );
        assert_equals( *address.point_to_point_destination(), IPv4Address( "209.53.255.1" ) );

        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Anycast ) );
        assert_that( address.is_flag_enabled( InterfaceIPAddressFlag::AutoConfigured ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Deprecated ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Detached ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Duplicated ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Dynamic ) );
        assert_that( address.is_flag_enabled( InterfaceIPAddressFlag::NoDad ) );
        assert_that( address.is_flag_enabled( InterfaceIPAddressFlag::Optimistic ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Secured ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Temporary ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Tentative ) );
    }

    void
    test_simple_v6()
    {
        InterfaceIPAddress address( IPv6Address( "2001::dead:beef" ), 0, 64 );
        assert_equals( address.address(), IPv6Address( "2001::dead:beef" ) );
        assert_equals( *address.prefix_length(), 64 );
        assert_not_that(
            static_cast< bool >( address.broadcast_address() ),
            "There should be no broadcast address." );
        assert_not_that(
            static_cast< bool >( address.point_to_point_destination() ),
            "There should be no P2P destination." );

        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Anycast ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::AutoConfigured ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Deprecated ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Detached ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Duplicated ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Dynamic ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::NoDad ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Optimistic ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Secured ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Temporary ) );
        assert_not_that( address.is_flag_enabled( InterfaceIPAddressFlag::Tentative ) );
    }

    [[maybe_unused]]
    static
    std::unique_ptr<Test>
    create()
    {
        return std::make_unique< TestInterfaceIPAddress >();
    }
};

namespace
{
    [[maybe_unused]]
    Tests::Test::Registrar< TestInterfaceIPAddress > registrar( "TestInterfaceIPAddress" );
}
