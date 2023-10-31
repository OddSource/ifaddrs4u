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

#include "../Interface.h"
#include "main.h"

using namespace OddSource::Interfaces;

class TestInterfaceIPAddress : public Tests::Test
{
public:
    TestInterfaceIPAddress()
        : Test()
    {
        add_test(test_simple_v4);
        add_test(test_v4_with_broadcast);
        add_test(test_v4_with_p2p);
        add_test(test_simple_v6);
    }

    void test_simple_v4()
    {
        InterfaceIPAddress<IPv4Address> address(IPv4Address("201.17.159.33"), 0, 24);
        assert_equals(address.address(), IPv4Address("201.17.159.33"));
        assert_equals(*address.prefix_length(), 24);
        assert_not_that((bool)address.broadcast_address(), "There should be no broadcast address.");
        assert_not_that((bool)address.point_to_point_destination(), "There should be no P2P destination.");

        assert_not_that(address.is_flag_enabled(Anycast));
        assert_not_that(address.is_flag_enabled(AutoConfigured));
        assert_not_that(address.is_flag_enabled(Deprecated));
        assert_not_that(address.is_flag_enabled(Detached));
        assert_not_that(address.is_flag_enabled(Duplicated));
        assert_not_that(address.is_flag_enabled(Dynamic));
        assert_not_that(address.is_flag_enabled(NoDad));
        assert_not_that(address.is_flag_enabled(Optimistic));
        assert_not_that(address.is_flag_enabled(Secured));
        assert_not_that(address.is_flag_enabled(Temporary));
        assert_not_that(address.is_flag_enabled(Tentative));
    }

    void test_v4_with_broadcast()
    {
        InterfaceIPAddress<IPv4Address> address(
                IPv4Address("209.53.101.102"), Dynamic | Secured, 16, IPv4Address("209.53.255.254"));
        assert_equals(address.address(), IPv4Address("209.53.101.102"));
        assert_equals(*address.prefix_length(), 16);
        assert_equals(*address.broadcast_address(), IPv4Address("209.53.255.254"));
        assert_not_that((bool)address.point_to_point_destination(), "There should be no P2P destination.");

        assert_not_that(address.is_flag_enabled(Anycast));
        assert_not_that(address.is_flag_enabled(AutoConfigured));
        assert_not_that(address.is_flag_enabled(Deprecated));
        assert_not_that(address.is_flag_enabled(Detached));
        assert_not_that(address.is_flag_enabled(Duplicated));
        assert_that(address.is_flag_enabled(Dynamic));
        assert_not_that(address.is_flag_enabled(NoDad));
        assert_not_that(address.is_flag_enabled(Optimistic));
        assert_that(address.is_flag_enabled(Secured));
        assert_not_that(address.is_flag_enabled(Temporary));
        assert_not_that(address.is_flag_enabled(Tentative));
    }

    void test_v4_with_p2p()
    {
        InterfaceIPAddress<IPv4Address> address(
                IPv4Address("209.53.101.102"),
                AutoConfigured | NoDad | Optimistic,
                0,
                IPv4Address("209.53.255.1"),
                true);
        assert_equals(address.address(), IPv4Address("209.53.101.102"));
        assert_not_that((bool)address.prefix_length(), "There should be no prefix length.");
        assert_not_that((bool)address.broadcast_address(), "There should be no broadcast address.");
        assert_equals(*address.point_to_point_destination(), IPv4Address("209.53.255.1"));

        assert_not_that(address.is_flag_enabled(Anycast));
        assert_that(address.is_flag_enabled(AutoConfigured));
        assert_not_that(address.is_flag_enabled(Deprecated));
        assert_not_that(address.is_flag_enabled(Detached));
        assert_not_that(address.is_flag_enabled(Duplicated));
        assert_not_that(address.is_flag_enabled(Dynamic));
        assert_that(address.is_flag_enabled(NoDad));
        assert_that(address.is_flag_enabled(Optimistic));
        assert_not_that(address.is_flag_enabled(Secured));
        assert_not_that(address.is_flag_enabled(Temporary));
        assert_not_that(address.is_flag_enabled(Tentative));
    }

    void test_simple_v6()
    {
        InterfaceIPAddress<IPv6Address> address(IPv6Address("2001::dead:beef"), 0, 64);
        assert_equals(address.address(), IPv6Address("2001::dead:beef"));
        assert_equals(*address.prefix_length(), 64);
        assert_not_that((bool)address.broadcast_address(), "There should be no broadcast address.");
        assert_not_that((bool)address.point_to_point_destination(), "There should be no P2P destination.");

        assert_not_that(address.is_flag_enabled(Anycast));
        assert_not_that(address.is_flag_enabled(AutoConfigured));
        assert_not_that(address.is_flag_enabled(Deprecated));
        assert_not_that(address.is_flag_enabled(Detached));
        assert_not_that(address.is_flag_enabled(Duplicated));
        assert_not_that(address.is_flag_enabled(Dynamic));
        assert_not_that(address.is_flag_enabled(NoDad));
        assert_not_that(address.is_flag_enabled(Optimistic));
        assert_not_that(address.is_flag_enabled(Secured));
        assert_not_that(address.is_flag_enabled(Temporary));
        assert_not_that(address.is_flag_enabled(Tentative));
    }

    static std::unique_ptr<Test> create()
    {
        return std::make_unique<TestInterfaceIPAddress>();
    }
};

namespace
{
    [[maybe_unused]]
    Tests::Test::Registrar<TestInterfaceIPAddress> registrar("TestInterfaceIPAddress");
}
