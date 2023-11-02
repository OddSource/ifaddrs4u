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

#include <cctype>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>

#include "MacAddress.h"
#include "s.h"

namespace
{
    using namespace OddSource::Interfaces;

    ::std::unique_ptr<uint8_t[]>
    copy_hw_addr(uint8_t const data[MAX_ADAPTER_ADDRESS_LENGTH], uint8_t data_length)
    {
        auto new_data = ::std::make_unique<uint8_t[]>(data_length);
        ::std::memcpy(new_data.get(), data, data_length);
        return new_data;
    }

    uint8_t predict_repr_length(::std::string_view const & repr)
    {
        uint8_t count(0);
        for (char const & c: repr)
        {
            if (c == ':' || c == '-')
            {
                count++;
            }
        }
        return count + 1;
    }

#define MAC_ADDR_REPR_POS ::std::string(repr) + "' at position "s + std::to_string(position)

    ::std::unique_ptr<uint8_t[]>
    from_repr(::std::string_view const & repr)
    {
        // GNUC has ether_aton_r, which is thread-safe, but BSD systems have
        // ether_aton, which is not thread safe and basically cannot safely be used.
        // Windows has no built-in method until C#. So ... let's try something simple-ish.
        auto predicted_length((size_t) predict_repr_length(repr));
        if (predicted_length > MAX_ADAPTER_ADDRESS_LENGTH)
        {
            ::std::ostringstream oss;
            oss << "MAC address length (" << predicted_length
                << " bytes) too long (max " << MAX_ADAPTER_ADDRESS_LENGTH << " bytes).";
            throw InvalidMacAddress(oss.str());
        }
        if (predicted_length < MIN_ADAPTER_ADDRESS_LENGTH)
        {
            ::std::ostringstream oss;
            oss << "MAC address length (" << predicted_length
                 << " bytes) too short (min " << MIN_ADAPTER_ADDRESS_LENGTH << " bytes).";
            throw InvalidMacAddress(oss.str());
        }

        auto data = ::std::make_unique<uint8_t[]>(MAX_ADAPTER_ADDRESS_LENGTH);
        size_t size(0), position(0);
        uint8_t byte(0), chars_in_byte(0);
        for (char c : repr)
        {
            char ch = (char)::std::tolower(c);
            if (ch == ':' || ch == '-')
            {
                if (chars_in_byte != 2)
                {
                    throw InvalidMacAddress("Unexpected separator in MAC address '"s + MAC_ADDR_REPR_POS);
                }
                data[size++] = byte;
                byte = chars_in_byte = 0;
            }
            else if ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f'))
            {
                chars_in_byte++;
                if (chars_in_byte > 2)
                {
                    throw InvalidMacAddress(
                        "Invalid number of characters "s + ::std::to_string(chars_in_byte) +
                        " between separators in MAC address '"s + MAC_ADDR_REPR_POS);
                }
                byte <<= 4;
                byte += ::std::isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);
            }
            else
            {
                throw InvalidMacAddress("Invalid character '"s + ch + "' in MAC address "s + MAC_ADDR_REPR_POS);
            }

            position++;
        }
        if (chars_in_byte != 2)
        {
            throw InvalidMacAddress("Unexpected separator in MAC address '"s + MAC_ADDR_REPR_POS);
        }
        data[size++] = byte;
        if(size != predicted_length)
        {
            throw InvalidMacAddress(
                "Malformed MAC address '"s + ::std::string(repr) + "' did not match expected length "s +
                ::std::to_string(predicted_length) + "."s);
        }

        return data;
    }

    ::std::string
    to_repr(uint8_t const data[MAX_ADAPTER_ADDRESS_LENGTH], uint8_t data_length)
    {
        ::std::ostringstream oss;
        if (data_length > MAX_ADAPTER_ADDRESS_LENGTH)
        {
            oss << "MAC address length " << data_length << " greater than allowed length "
                << MAX_ADAPTER_ADDRESS_LENGTH;
            throw InvalidMacAddress(oss.str());
        }
        if (data_length < MIN_ADAPTER_ADDRESS_LENGTH)
        {
            oss << "MAC address length (" << data_length
                << " bytes) too short (min " << MIN_ADAPTER_ADDRESS_LENGTH << " bytes).";
            throw InvalidMacAddress(oss.str());
        }

        static const uint8_t formatted_byte_size(3);

        auto buffer = new char[formatted_byte_size];
        for(uint8_t i(0); i < data_length; i++)
        {
            ::std::snprintf( buffer, formatted_byte_size, "%02x", data[i] );
            if (i > 0)
            {
                oss << ':';
            }
            oss << buffer;
        }
        delete[] buffer;

        return oss.str();
    }
}

struct OddSource::Interfaces::MacTempDataHolder
{
    mutable ::std::unique_ptr<uint8_t[]> data;
    uint8_t length;
};

OddSource::Interfaces::MacAddress::
MacAddress(MacAddress const & other)
    : _representation(other._representation),
      _data(copy_hw_addr(other._data.get(), other._data_length)),
      _data_length(other._data_length)
{
}

OddSource::Interfaces::MacAddress::
MacAddress(::std::string_view const & repr)
    : MacAddress(repr, {from_repr(repr), predict_repr_length(repr)})
{
}

OddSource::Interfaces::MacAddress::
MacAddress(uint8_t const data[MAX_ADAPTER_ADDRESS_LENGTH], uint8_t data_length)
    : MacAddress(to_repr(data, data_length), {copy_hw_addr(data, data_length), data_length})
{
}

OddSource::Interfaces::MacAddress::
MacAddress(::std::string_view const & repr, MacTempDataHolder const & holder)
    : _representation(repr),
      _data(::std::move(holder.data)),
      _data_length(holder.length)
{
}
