#include "../MacAddress.h"
#include "main.h"

using namespace OddSource::Interfaces;

class TestMacAddress : public Tests::Test
{
public:
    TestMacAddress()
        : Test()
    {
        add_test(test_string_round_trip);
        add_test(test_data_round_trip);
    }

    void test_string_round_trip()
    {
        assert_equals(::std::string(MacAddress("a4:83:e7:2e:a1:67")), "a4:83:e7:2e:a1:67");
        assert_equals(
            ::strcmp((char const *)MacAddress("b2:fb:b8:5b:84:e8:ff"), "b2:fb:b8:5b:84:e8:ff"),
            0,
            "The C strings do not match");

        ::std::ostringstream oss;
        oss << MacAddress("82:1c:78:44:5c:05");
        assert_equals(oss.str(), "82:1c:78:44:5c:05");
    }

    void test_data_round_trip()
    {
        static uint8_t const data1[6] {0xa4, 0xe7, 0x83, 0xa1, 0x2e, 0x67};
        static uint8_t const data2[7] {0xb2, 0xb8, 0xfb, 0x84, 0x5b, 0xff, 0xe8};

        {
            MacAddress address(data1, 6);
            assert_equals(::std::string(address), "a4:e7:83:a1:2e:67");
            assert_equals(address.length(), 6);

            auto data((uint8_t const *)address);

            for(size_t i = 0; i < 6; i++)
            {
                assert_equals(data[i], data1[i], ::std::string("Bytes ") + ::std::to_string(i) + " do not match.");
            }
        }

        {
            MacAddress address(data2, 7);
            assert_equals(::std::string(address), "b2:b8:fb:84:5b:ff:e8");
            assert_equals(address.length(), 7);

            auto data((uint8_t const *)address);

            for(size_t i = 0; i < 7; i++)
            {
                assert_equals(data[i], data2[i], ::std::string("Bytes ") + ::std::to_string(i) + " do not match.");
            }
        }
    }

    static std::unique_ptr<Test> create()
    {
        return std::make_unique<TestMacAddress>();
    }
};

namespace
{
    [[maybe_unused]]
    Tests::Test::Registrar<TestMacAddress> registrar("TestMacAddress");
}
