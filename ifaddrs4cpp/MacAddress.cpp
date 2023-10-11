#include <cctype>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>

#include "MacAddress.h"

namespace
{
    uint8_t const *
    copy_hw_addr(uint8_t const data[MAX_ADAPTER_ADDRESS_LENGTH], uint8_t data_length)
    {
        auto new_data = new uint8_t[data_length];
        ::std::memcpy(new_data, data, data_length);
        return new_data;
    }
}

OddSource::Interfaces::MacAddress::
MacAddress(MacAddress const & other)
    : _representation(other._representation),
      _data(nullptr),
      _data_length(other._data_length)
{
    delete[] this->_data;
    this->_data = copy_hw_addr(other._data, other._data_length);
}

OddSource::Interfaces::MacAddress::
MacAddress(MacAddress && other) noexcept
    : _representation(other._representation),
      _data(::std::move(other._data)),
      _data_length(other._data_length)
{
}

OddSource::Interfaces::MacAddress::
MacAddress(::std::string_view const & repr)
    : MacAddress(repr, MacAddress::from_repr(repr), ADAPTER_ADDRESS_FROM_REPR_LENGTH)
{
}

OddSource::Interfaces::MacAddress::
MacAddress(uint8_t const data[MAX_ADAPTER_ADDRESS_LENGTH], uint8_t data_length)
    : MacAddress(MacAddress::to_repr(data, data_length), copy_hw_addr(data, data_length), data_length)
{
}

OddSource::Interfaces::MacAddress::
MacAddress(::std::string_view const & repr, uint8_t const * data, uint8_t data_length)
    : _representation(repr),
      _data(data),
      _data_length(data_length)
{
}

OddSource::Interfaces::MacAddress::
~MacAddress()
{
    delete[] this->_data;
}

#define WRONG_CHAR_THROW { \
            if ((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f')) \
            { \
                delete[] data; \
                throw InvalidMacAddress(( \
                    ::std::ostringstream() << "MAC address character out of range: " << ch \
                ).str()); \
            } \
        }

uint8_t const *
OddSource::Interfaces::MacAddress::
from_repr(::std::string_view const & repr)
{
    // GNUC has ether_aton_r, which is thread-safe, but BSD systems have
    // ether_aton, which is not thread safe and basically cannot safely be used.
    // Windows has no built-in method until C#. So ... let's try something simple-ish.
    auto data = new uint8_t [ADAPTER_ADDRESS_FROM_REPR_LENGTH];
    size_t size;
    auto c = repr.begin();
    auto end = repr.end();
    for (size = 0; size < ADAPTER_ADDRESS_FROM_REPR_LENGTH; size++)
    {
        uint8_t byte;

        char ch = (char)::std::tolower(*c++);
        WRONG_CHAR_THROW
        byte = ::std::isdigit (ch) ? (ch - '0') : (ch - 'a' + 10);

        if (c != end)
        {
            ch = (char)::std::tolower(*c);
            if ((size < ADAPTER_ADDRESS_FROM_REPR_LENGTH - 1 && ch != ':') ||
                (size == ADAPTER_ADDRESS_FROM_REPR_LENGTH - 1 && !::std::isspace(ch)))
            {
                c++;
                WRONG_CHAR_THROW
                byte <<= 4;
                byte += ::std::isdigit (ch) ? (ch - '0') : (ch - 'a' + 10);
                ch = *c;
                if (size < ADAPTER_ADDRESS_FROM_REPR_LENGTH -1 && ch != ':')
                {
                    delete[] data;
                    throw InvalidMacAddress("Malformed MAC address, expected ':' between bytes.");
                }
            }
        }
        else if(size < ADAPTER_ADDRESS_FROM_REPR_LENGTH - 1)
        {
            throw InvalidMacAddress("Malformed MAC address is not long enough.");
        }

        data[size] = byte;
        c++;
    }
    return data;
}

::std::string
OddSource::Interfaces::MacAddress::
to_repr(uint8_t const data[MAX_ADAPTER_ADDRESS_LENGTH], uint8_t data_length)
{
    if (data_length > MAX_ADAPTER_ADDRESS_LENGTH)
    {
        throw InvalidMacAddress((
            ::std::ostringstream() << "MAC address length " << data_length << " greater than allowed length "
                                   << MAX_ADAPTER_ADDRESS_LENGTH
        ).str());
    }

    static const uint8_t formatted_byte_size(3);

    ::std::ostringstream oss;
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
