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

namespace OddSource::Interfaces
{
    class TestInterface : public Tests::Test
    {
    public:
        TestInterface()
            : Test()
        {
            add_test(test_empty_flags);
            add_test(test_some_flags);
            add_test(test_other_flags);
            add_test(test_mac_address);
            add_test(test_ipv4_addresses);
            add_test(test_ipv6_addresses);
        }

        void test_empty_flags()
        {
            Interface iface(
                    12, "eth1",
#ifdef IS_WINDOWS
                    "c9cc6972-a12b-43d6-9f99-fd7ea946ba5a",
#endif /* IS_WINDOWS */
                    0);

            assert_equals(iface.index(), 12u);
            assert_equals(iface.name(), "eth1");
#ifdef IS_WINDOWS
            assert_equals(iface.windows_uuid(), "c9cc6972-a12b-43d6-9f99-fd7ea946ba5a");
#endif /* IS_WINDOWS */

            assert_not_that((bool)iface.mtu());
            assert_not_that(iface.is_up());
            assert_not_that(iface.is_loopback());

            assert_not_that(iface.is_flag_enabled(BroadcastAddressSet));
            assert_not_that(iface.is_flag_enabled(DebugEnabled));
            assert_not_that(iface.is_flag_enabled(IsLoopback));
            assert_not_that(iface.is_flag_enabled(IsPointToPoint));
            assert_not_that(iface.is_flag_enabled(IsRunning));
            assert_not_that(iface.is_flag_enabled(IsUp));
            assert_not_that(iface.is_flag_enabled(NoARP));
            assert_not_that(iface.is_flag_enabled(NoTrailers));
            assert_not_that(iface.is_flag_enabled(PromiscuousModeEnabled));
            assert_not_that(iface.is_flag_enabled(ReceiveAllMulticastPackets));
            assert_not_that(iface.is_flag_enabled(SupportsMulticast));

            assert_not_that(iface.has_mac_address());
            assert_not_that((bool)iface.mac_address());
            assert_equals(iface.ipv4_addresses().size(), 0u);
            assert_equals(iface.ipv6_addresses().size(), 0u);
        }

        void test_some_flags()
        {
            Interface iface(
                    1, "lo0",
#ifdef IS_WINDOWS
                    "9a33c41c-3312-495a-a882-1a5420d26d17",
#endif /* IS_WINDOWS */
                    BroadcastAddressSet | IsLoopback | IsRunning | NoARP | ReceiveAllMulticastPackets,
                    1750);

            assert_equals(iface.index(), 1u);
            assert_equals(iface.name(), "lo0");
#ifdef IS_WINDOWS
            assert_equals(iface.windows_uuid(), "9a33c41c-3312-495a-a882-1a5420d26d17");
#endif /* IS_WINDOWS */

            assert_equals(*iface.mtu(), 1750u);
            assert_not_that(iface.is_up());
            assert_that(iface.is_loopback());

            assert_that(iface.is_flag_enabled(BroadcastAddressSet));
            assert_not_that(iface.is_flag_enabled(DebugEnabled));
            assert_that(iface.is_flag_enabled(IsLoopback));
            assert_not_that(iface.is_flag_enabled(IsPointToPoint));
            assert_that(iface.is_flag_enabled(IsRunning));
            assert_not_that(iface.is_flag_enabled(IsUp));
            assert_that(iface.is_flag_enabled(NoARP));
            assert_not_that(iface.is_flag_enabled(NoTrailers));
            assert_not_that(iface.is_flag_enabled(PromiscuousModeEnabled));
            assert_that(iface.is_flag_enabled(ReceiveAllMulticastPackets));
            assert_not_that(iface.is_flag_enabled(SupportsMulticast));

            assert_not_that(iface.has_mac_address());
            assert_not_that((bool)iface.mac_address());
            assert_equals(iface.ipv4_addresses().size(), 0u);
            assert_equals(iface.ipv6_addresses().size(), 0u);
        }

        void test_other_flags()
        {
            Interface iface(
                    3, "en0",
#ifdef IS_WINDOWS
                    "b01c844e-0bb5-48fe-bc99-ddac9a284b80",
#endif /* IS_WINDOWS */
                    DebugEnabled | IsPointToPoint | IsUp | NoTrailers | PromiscuousModeEnabled | SupportsMulticast);

            assert_equals(iface.index(), 3u);
            assert_equals(iface.name(), "en0");
#ifdef IS_WINDOWS
            assert_equals(iface.windows_uuid(), "b01c844e-0bb5-48fe-bc99-ddac9a284b80");
#endif /* IS_WINDOWS */

            assert_not_that((bool)iface.mtu());
            assert_that(iface.is_up());
            assert_not_that(iface.is_loopback());

            assert_not_that(iface.is_flag_enabled(BroadcastAddressSet));
            assert_that(iface.is_flag_enabled(DebugEnabled));
            assert_not_that(iface.is_flag_enabled(IsLoopback));
            assert_that(iface.is_flag_enabled(IsPointToPoint));
            assert_not_that(iface.is_flag_enabled(IsRunning));
            assert_that(iface.is_flag_enabled(IsUp));
            assert_not_that(iface.is_flag_enabled(NoARP));
            assert_that(iface.is_flag_enabled(NoTrailers));
            assert_that(iface.is_flag_enabled(PromiscuousModeEnabled));
            assert_not_that(iface.is_flag_enabled(ReceiveAllMulticastPackets));
            assert_that(iface.is_flag_enabled(SupportsMulticast));

            assert_not_that(iface.has_mac_address());
            assert_not_that((bool)iface.mac_address());
            assert_equals(iface.ipv4_addresses().size(), 0u);
            assert_equals(iface.ipv6_addresses().size(), 0u);
        }

        void test_mac_address()
        {
            Interface iface(
                    12, "eth1",
#ifdef IS_WINDOWS
                    "c9cc6972-a12b-43d6-9f99-fd7ea946ba5a",
#endif /* IS_WINDOWS */
                    0);

            assert_not_that(iface.has_mac_address());
            assert_not_that((bool)iface.mac_address());
            assert_equals(iface.ipv4_addresses().size(), 0u);
            assert_equals(iface.ipv6_addresses().size(), 0u);

            iface._mac_address.emplace("82:1c:78:44:5c:01");

            assert_that(iface.has_mac_address());
            assert_equals(*iface.mac_address(), MacAddress("82:1c:78:44:5c:01"));
            assert_equals(iface.ipv4_addresses().size(), 0u);
            assert_equals(iface.ipv6_addresses().size(), 0u);
        }

        void test_ipv4_addresses()
        {
            Interface iface(
                    12, "eth1",
#ifdef IS_WINDOWS
                    "c9cc6972-a12b-43d6-9f99-fd7ea946ba5a",
#endif /* IS_WINDOWS */
                    0);

            assert_not_that(iface.has_mac_address());
            assert_not_that((bool)iface.mac_address());
            assert_equals(iface.ipv4_addresses().size(), 0u);
            assert_equals(iface.ipv6_addresses().size(), 0u);

            iface._ipv4_addresses.emplace_back(IPv4Address("192.168.0.52"), 0, 24);
            iface._ipv4_addresses.emplace_back(IPv4Address("192.168.0.53"), 0, 24);

            assert_not_that(iface.has_mac_address());
            assert_not_that((bool)iface.mac_address());
            assert_equals(iface.ipv4_addresses().size(), 2u);
            assert_equals(iface.ipv6_addresses().size(), 0u);

            assert_equals(iface.ipv4_addresses()[0],
                          InterfaceIPAddress<IPv4Address>(IPv4Address("192.168.0.52"), 0, 24));
            assert_equals(iface.ipv4_addresses()[1],
                          InterfaceIPAddress<IPv4Address>(IPv4Address("192.168.0.53"), 0, 24));
        }

        void test_ipv6_addresses()
        {
            Interface iface(
                    12, "eth1",
#ifdef IS_WINDOWS
                    "c9cc6972-a12b-43d6-9f99-fd7ea946ba5a",
#endif /* IS_WINDOWS */
                    0);

            assert_not_that(iface.has_mac_address());
            assert_not_that((bool)iface.mac_address());
            assert_equals(iface.ipv4_addresses().size(), 0u);
            assert_equals(iface.ipv6_addresses().size(), 0u);

            iface._ipv6_addresses.emplace_back(IPv6Address("2001::dead:beef"), 0, 64);
            iface._ipv6_addresses.emplace_back(IPv6Address("2001:471:c2bd:bb61:6d7b:48a5:6304:31e5"), 0, 64);
            iface._ipv6_addresses.emplace_back(IPv6Address("fe80::f1:1612:447b:70c5%en0"), 0, 64);

            assert_not_that(iface.has_mac_address());
            assert_not_that((bool)iface.mac_address());
            assert_equals(iface.ipv4_addresses().size(), 0u);
            assert_equals(iface.ipv6_addresses().size(), 3u);

            assert_equals(iface.ipv6_addresses()[0],
                          InterfaceIPAddress<IPv6Address>(IPv6Address("2001::dead:beef"), 0, 64));
            assert_equals(iface.ipv6_addresses()[1],
                          InterfaceIPAddress<IPv6Address>(IPv6Address("2001:471:c2bd:bb61:6d7b:48a5:6304:31e5"), 0, 64));
            assert_equals(iface.ipv6_addresses()[2],
                          InterfaceIPAddress<IPv6Address>(IPv6Address("fe80::f1:1612:447b:70c5%en0"), 0, 64));
        }

        static std::unique_ptr<Test> create()
        {
            return std::make_unique<TestInterface>();
        }
    };
}

namespace
{
    [[maybe_unused]]
    OddSource::Interfaces::Tests::Test::Registrar<OddSource::Interfaces::TestInterface> registrar("TestInterface");
}
