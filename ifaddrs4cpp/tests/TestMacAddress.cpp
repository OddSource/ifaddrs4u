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

#include "../MacAddress.h"
#include "main.h"

#include <cstring>

using namespace OddSource::Interfaces;

class TestMacAddress : public Tests::Test
{
public:
    TestMacAddress()
        : Test()
    {
        add_test(test_equals);
        add_test(test_string_round_trip);
        add_test(test_data_round_trip);
        add_test(test_construct_malformed);
    }

    void test_equals()
    {
        assert_equals(MacAddress("a4:83:e7:2e:a1:67"), MacAddress("a4:83:e7:2e:a1:67"));
        assert_equals(MacAddress("a4:83:e7:2e:a1:67"), MacAddress("A4:83:E7:2E:A1:67"));
        assert_not_equals(MacAddress("b2:fb:b8:5b:84:e8"), MacAddress("b2:fb:b8:5b:84:e8:ff"));
        assert_not_equals(MacAddress("b2:fb:b8:5b:84:e8:fe"), MacAddress("b2:fb:b8:5b:84:e8:ff"));
    }

    void test_string_round_trip()
    {
        assert_equals((::std::string)MacAddress("a4:83:e7:2e:a1:67"), "a4:83:e7:2e:a1:67");
        assert_equals(
            ::strcmp((char const *)MacAddress("b2:fb:b8:5b:84:e8:ff"), "b2:fb:b8:5b:84:e8:ff"),
            0,
            "The C strings do not match");

        assert_equals((::std::string)MacAddress("a4-83-e7-2e-a1-67"), "a4-83-e7-2e-a1-67");

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
            assert_equals((::std::string)address, "a4:e7:83:a1:2e:67");
            assert_equals(address.length(), 6);

            auto data((uint8_t const *)address);

            for(size_t i = 0; i < 6; i++)
            {
                assert_equals(data[i], data1[i], "Bytes "s + ::std::to_string(i) + " do not match."s);
            }
        }

        {
            MacAddress address(data2, 7);
            assert_equals((::std::string)address, "b2:b8:fb:84:5b:ff:e8");
            assert_equals(address.length(), 7);

            auto data((uint8_t const *)address);

            for(size_t i = 0; i < 7; i++)
            {
                assert_equals(data[i], data2[i], "Bytes "s + ::std::to_string(i) + " do not match."s);
            }
        }
    }

    void test_construct_malformed()
    {
        assert_throws(MacAddress("67:a1:2e:ff:e5"), ::std::invalid_argument);
        assert_throws(MacAddress("67:a1:2e:ff:e5:e6:e7:e8:e9"), ::std::invalid_argument);
        assert_throws(MacAddress("67a12ef1a4e7"), ::std::invalid_argument);

        static uint8_t const data1[5] {0xa4, 0xe7, 0x83, 0xa1, 0x2e};
        static uint8_t const data2[9] {0xb2, 0xb8, 0xfb, 0x84, 0x5b, 0xff, 0xe8, 0xe7, 0xbb};

        assert_throws(MacAddress((uint8_t const *)data1, 5), ::std::invalid_argument);
        assert_throws(MacAddress((uint8_t const *)data2, 9), ::std::invalid_argument);
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
