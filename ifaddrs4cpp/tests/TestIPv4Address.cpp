#include "../IpAddress.h"
#include "main.h"

using namespace OddSource::Interfaces;

class TestIPv4Address : public Tests::Test
{
public:
    TestIPv4Address()
        : Test()
    {
        add_test(test_string_round_trip);
        add_test(test_in_addr_round_trip);
        add_test(test_unspecified_address);
        add_test(test_loopback_addresses);
        add_test(test_link_local_addresses);
        add_test(test_multicast_addresses);
        add_test(test_private_addresses);
        add_test(test_other_reserved_addresses);
    }

    void test_string_round_trip()
    {
        assert_equals(::std::string(IPv4Address("4.3.5.6")), "4.3.5.6");
        assert_equals(::strcmp((char const *)IPv4Address("4.3.5.6"), "4.3.5.6"), 0, "The C strings do not match.");

        assert_equals(::std::string(IPv4Address("226.000.000.037")), "226.0.0.31"); // octal
        assert_equals(::std::string(IPv4Address("0x11.0x1b.0xf3.0x01")), "17.27.243.1"); // hexadecimal

        ::std::ostringstream oss;
        oss << IPv4Address("172.19.52.141");
        assert_equals(oss.str(), "172.19.52.141");
    }

    void test_in_addr_round_trip()
    {
        in_addr data {};
        inet_pton(AF_INET, "192.0.2.33", &data);
        IPv4Address address(&data);
        assert_equals(::std::string(address), "192.0.2.33");

        assert_equals(address.version(), IPv4);
        assert_equals(address.maximum_prefix_length(), 32);

        auto owned = (in_addr const *)address;
        auto bytes1 = reinterpret_cast<uint8_t const *>(owned);
        auto bytes2 = reinterpret_cast<uint8_t const *>(&data);

        for(size_t i = 0; i < sizeof(in_addr); i++)
        {
            assert_equals(bytes1[i], bytes2[i], ::std::string("Bytes ") + ::std::to_string(i) + " do not match.");
        }
    }

    void test_unspecified_address()
    {
        IPv4Address address("0.0.0.0");
        assert_that(address.is_unspecified(), "0.0.0.0 should be unspecified.");
        assert_that(address.is_reserved(), "0.0.0.0 should be reserved.");
        assert_that(!address.is_private(), "0.0.0.0 should not be private.");
        assert_that(!address.is_loopback(), "0.0.0.0 should not be a loopback.");
        assert_that(!address.is_link_local(), "0.0.0.0 should not be link-local.");
        assert_that(!address.is_multicast(), "0.0.0.0 should not be multicast.");

        assert_that(!IPv4Address("0.0.0.1").is_unspecified(), "0.0.0.1 should not be unspecified.");
    }

    void test_loopback_addresses()
    {
        IPv4Address address("127.0.0.0");
        assert_that(address.is_loopback(), "127.0.0.0 should be a loopback.");
        assert_that(address.is_reserved(), "127.0.0.0 should be reserved.");
        assert_that(!address.is_private(), "127.0.0.0 should not be private.");
        assert_that(!address.is_unspecified(), "127.0.0.0 should not be a unspecified.");
        assert_that(!address.is_link_local(), "127.0.0.0 should not be link-local.");
        assert_that(!address.is_multicast(), "127.0.0.0 should not be multicast.");

        assert_that(IPv4Address("127.0.0.1").is_loopback(), "127.0.0.1 should be a loopback.");
        assert_that(IPv4Address("127.0.0.124").is_loopback(), "127.0.0.124 should be a loopback.");
        assert_that(IPv4Address("127.0.0.255").is_loopback(), "127.0.0.255 should be a loopback.");
        assert_that(IPv4Address("127.255.255.255").is_loopback(), "127.255.255.255 should be a loopback.");
    }

    void test_link_local_addresses()
    {
        IPv4Address address("169.254.0.0");
        assert_that(address.is_link_local(), "169.254.0.0 should be link-local.");
        assert_that(address.is_reserved(), "169.254.0.0 should be reserved.");
        assert_that(!address.is_private(), "169.254.0.0 should not be private.");
        assert_that(!address.is_unspecified(), "169.254.0.0 should not be unspecified.");
        assert_that(!address.is_loopback(), "169.254.0.0 should not be a loopback.");
        assert_that(!address.is_multicast(), "169.254.0.0 should not be multicast.");

        assert_that(IPv4Address("169.254.0.1").is_link_local(), "169.254.0.1 should be link-local.");
        assert_that(IPv4Address("169.254.0.124").is_link_local(), "169.254.0.124 should be link-local.");
        assert_that(IPv4Address("169.254.0.255").is_link_local(), "169.254.0.255 should be link-local.");
        assert_that(IPv4Address("169.254.255.255").is_link_local(), "169.254.255.255 should be link-local.");
    }

    void test_multicast_addresses()
    {
        IPv4Address address("224.0.0.0");
        assert_that(address.is_multicast(), "224.0.0.0 should be multicast.");
        assert_that(address.is_reserved(), "224.0.0.0 should be reserved.");
        assert_that(!address.is_private(), "224.0.0.0 should not be private.");
        assert_that(!address.is_unspecified(), "224.0.0.0 should not be unspecified.");
        assert_that(!address.is_loopback(), "224.0.0.0 should not be a loopback.");
        assert_that(!address.is_link_local(), "224.0.0.0 should not be link-local.");

        assert_that(IPv4Address("224.0.0.1").is_multicast(), "224.0.0.1 should be multicast.");
        assert_that(IPv4Address("224.0.0.124").is_multicast(), "224.0.0.124 should be multicast.");
        assert_that(IPv4Address("224.0.0.255").is_multicast(), "224.0.0.255 should be multicast.");
        assert_that(IPv4Address("239.0.0.1").is_multicast(), "239.0.0.1 should be multicast.");
        assert_that(IPv4Address("239.255.255.255").is_multicast(), "239.255.255.255 should be multicast.");

        assert_that(!IPv4Address("192.168.0.1").multicast_scope(), "192.168.0.1 should not have a multicast scope.");
        assert_equals(*IPv4Address("224.0.0.0").multicast_scope(), LinkLocal);
        assert_equals(*IPv4Address("224.0.0.255").multicast_scope(), LinkLocal);
        assert_equals(*IPv4Address("224.0.1.0").multicast_scope(), Global);
        assert_equals(*IPv4Address("238.255.255.255").multicast_scope(), Global);
        assert_equals(*IPv4Address("239.0.0.0").multicast_scope(), Unassigned);
        assert_equals(*IPv4Address("239.191.255.255").multicast_scope(), Unassigned);
        assert_equals(*IPv4Address("239.192.0.0").multicast_scope(), OrganizationLocal);
        assert_equals(*IPv4Address("239.192.255.255").multicast_scope(), OrganizationLocal);
        assert_equals(*IPv4Address("239.195.255.255").multicast_scope(), OrganizationLocal);
        assert_equals(*IPv4Address("239.196.0.0").multicast_scope(), Unassigned);
        assert_equals(*IPv4Address("239.254.255.255").multicast_scope(), Unassigned);
        assert_equals(*IPv4Address("239.255.0.0").multicast_scope(), RealmLocal);
        assert_equals(*IPv4Address("239.255.255.255").multicast_scope(), RealmLocal);
    }

    void test_private_addresses()
    {
        IPv4Address address("10.0.0.0");
        assert_that(address.is_private(), "10.0.0.0 should be private");
        assert_that(address.is_reserved(), "10.0.0.0 should be reserved.");
        assert_that(!address.is_multicast(), "10.0.0.0 should not be multicast.");
        assert_that(!address.is_unspecified(), "10.0.0.0 should not be unspecified.");
        assert_that(!address.is_loopback(), "10.0.0.0 should not be a loopback.");
        assert_that(!address.is_link_local(), "10.0.0.0 should not be link-local.");

        assert_that(!IPv4Address("9.255.255.255").is_private(), "9.255.255.255 should not be private.");
        assert_that(IPv4Address("10.0.0.1").is_private(), "10.0.0.1 should be private.");
        assert_that(IPv4Address("10.20.30.75").is_private(), "10.20.30.75 should be private.");
        assert_that(IPv4Address("10.255.255.255").is_private(), "10.255.255.255 should be private.");
        assert_that(!IPv4Address("11.0.0.0").is_private(), "11.0.0.0 should be not private.");

        assert_that(!IPv4Address("100.63.255.255").is_private(), "100.63.255.255 should not be private.");
        assert_that(IPv4Address("100.64.0.0").is_private(), "100.64.0.0 should be private.");
        assert_that(IPv4Address("100.99.30.15").is_private(), "100.99.30.15 should be private.");
        assert_that(IPv4Address("100.127.255.255").is_private(), "100.127.255.255 should be private.");
        assert_that(!IPv4Address("100.128.0.0").is_private(), "100.128.0.0 should not be private.");

        assert_that(!IPv4Address("172.15.255.255").is_private(), "172.15.255.255 should not be private.");
        assert_that(IPv4Address("172.16.0.0").is_private(), "172.16.0.0 should be private.");
        assert_that(IPv4Address("172.24.5.5").is_private(), "172.24.5.5 should be private.");
        assert_that(IPv4Address("172.31.255.255").is_private(), "172.31.255.255 should be private.");
        assert_that(!IPv4Address("172.32.0.0").is_private(), "172.32.0.0 should not be private.");

        assert_that(!IPv4Address("191.255.255.255").is_private(), "191.255.255.255 should not be private.");
        assert_that(IPv4Address("192.0.0.0").is_private(), "192.0.0.0 should be private.");
        assert_that(IPv4Address("192.0.0.255").is_private(), "192.0.0.255 should be private.");
        assert_that(!IPv4Address("192.0.1.0").is_private(), "192.0.1.0 should not be private.");

        assert_that(!IPv4Address("192.167.255.255").is_private(), "192.167.255.255 should not be private.");
        assert_that(IPv4Address("192.168.0.0").is_private(), "192.168.0.0 should be private.");
        assert_that(IPv4Address("192.168.255.255").is_private(), "192.168.255.255 should be private.");
        assert_that(!IPv4Address("192.169.0.0").is_private(), "192.169.0.0 should not be private.");

        assert_that(!IPv4Address("198.17.255.255").is_private(), "198.17.255.255 should not be private.");
        assert_that(IPv4Address("198.18.0.0").is_private(), "198.18.0.0 should be private.");
        assert_that(IPv4Address("198.18.255.255").is_private(), "198.18.255.255 should be private.");
        assert_that(IPv4Address("198.19.0.0").is_private(), "198.19.0.0 should be private.");
        assert_that(IPv4Address("198.19.255.255").is_private(), "198.19.255.255 should be private.");
        assert_that(!IPv4Address("198.20.0.0").is_private(), "198.20.0.0 should not be private.");
    }

    void test_other_reserved_addresses()
    {
        ::std::vector<::std::string const> tests {
            "192.0.2.0", "192.0.2.255",
            "192.88.99.0", "192.88.99.255",
            "198.51.100.0", "198.51.100.255",
            "203.0.113.0", "203.0.113.255",
            "240.0.0.0", "240.0.0.255", "240.0.255.255", "240.255.255.255",
            "248.0.0.0", "250.0.0.0", "252.0.0.0", "254.0.0.0",
            "255.255.255.254", "255.255.255.255",
        };
        for (auto const & test : tests)
        {
            IPv4Address address(test);
            assert_that(address.is_reserved(), test + " should be reserved.");
            assert_that(!address.is_private(), test + " should not be private");
            assert_that(!address.is_multicast(), test + " should not be multicast.");
            assert_that(!address.is_unspecified(), test + " should not be unspecified.");
            assert_that(!address.is_loopback(), test + " should not be a loopback.");
            assert_that(!address.is_link_local(), test + " should not be link-local.");
        }
    }

    static std::unique_ptr<Test> create()
    {
        return std::make_unique<TestIPv4Address>();
    }
};

namespace
{
    [[maybe_unused]]
    Tests::Test::Registrar<TestIPv4Address> registrar("TestIPv4Address");
}
