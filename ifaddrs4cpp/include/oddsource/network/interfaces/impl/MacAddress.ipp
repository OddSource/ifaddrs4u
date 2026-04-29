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

#pragma once

#ifndef IFADDRS4CPP_INLINE_SOURCE
#include "../MacAddress.hpp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

// ReSharper disable once CppUnusedIncludeDirective
#include <cctype>
// ReSharper disable once CppUnusedIncludeDirective
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <utility>

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace
{
    using namespace OddSource::Interfaces;

    ::std::unique_ptr< ::std::uint8_t[] >
    copyHardwareAddress(
        ::std::uint8_t const data[ MAX_ADAPTER_ADDRESS_LENGTH ],
        ::std::uint8_t dataLength )
    {
        auto new_data = ::std::make_unique< ::std::uint8_t[] >( dataLength );
        ::std::memcpy( new_data.get(), data, dataLength );
        return new_data;
    }

    ::std::uint8_t
    predictReprLength(
        ::std::string_view const & repr )
    {
        ::std::uint8_t count{ 0 };
        for ( char const & c: repr )
        {
            if ( c == ':' || c == '-' )
            {
                count++;
            }
        }
        return count + 1;
    }

#define MAC_ADDR_REPR_POS ::std::string( repr ) + "' at position "s + std::to_string( position )

    ::std::unique_ptr< ::std::uint8_t[] >
    fromRepr(
        ::std::string_view const & repr )
    {
        using namespace ::std::string_literals;
        // GNUC has ether_aton_r, which is thread-safe, but BSD systems have
        // ether_aton, which is not thread safe and basically cannot safely be used.
        // Windows has no built-in method until C#. So ... let's try something simple-ish.
        auto predicted_length( static_cast< size_t >( predictReprLength( repr ) ) );
        if (predicted_length > MAX_ADAPTER_ADDRESS_LENGTH)
        {
            ::std::ostringstream oss;
            oss << "MAC address length (" << ::std::to_string(predicted_length)
                << " bytes) too long (max " << ::std::to_string(MAX_ADAPTER_ADDRESS_LENGTH)
                << " bytes).";
            throw InvalidMacAddress(oss.str());
        }
        if (predicted_length < MIN_ADAPTER_ADDRESS_LENGTH)
        {
            ::std::ostringstream oss;
            oss << "MAC address length (" << ::std::to_string(predicted_length)
                 << " bytes) too short (min " << ::std::to_string(MIN_ADAPTER_ADDRESS_LENGTH)
                 << " bytes).";
            throw InvalidMacAddress(oss.str());
        }

        auto data = ::std::make_unique<::std::uint8_t[]>(MAX_ADAPTER_ADDRESS_LENGTH);
        size_t size(0), position(0);
        ::std::uint8_t byte(0), chars_in_byte(0);
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

#undef MAC_ADDR_REPR_POS

    ::std::string
    toRepr(
        ::std::uint8_t const data[MAX_ADAPTER_ADDRESS_LENGTH],
        ::std::uint8_t data_length )
    {
        ::std::ostringstream oss;
        if (data_length > MAX_ADAPTER_ADDRESS_LENGTH)
        {
            oss << "MAC address length (" << ::std::to_string(data_length)
                << ") greater than allowed length " << ::std::to_string(MAX_ADAPTER_ADDRESS_LENGTH);
            throw InvalidMacAddress(oss.str());
        }
        if (data_length < MIN_ADAPTER_ADDRESS_LENGTH)
        {
            oss << "MAC address length (" << ::std::to_string(data_length)
                << " bytes) too short (min " << ::std::to_string(MIN_ADAPTER_ADDRESS_LENGTH) << " bytes).";
            throw InvalidMacAddress(oss.str());
        }

        static const ::std::uint8_t formatted_byte_size(3);

        auto buffer = new char[formatted_byte_size];
        for(::std::uint8_t i(0); i < data_length; i++)
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

namespace OddSource::Interfaces
{
    OddSource_Inline
    InvalidMacAddress::
    InvalidMacAddress(
    ::std::string_view const & what )
    : ::std::invalid_argument( ::std::string( what ) )
    {
    }

    OddSource_Inline
    InvalidMacAddress::
    InvalidMacAddress(
        InvalidMacAddress const & other )
        : ::std::invalid_argument( other )
    {
    }

    OddSource_Inline
    InvalidMacAddress::
    ~InvalidMacAddress() noexcept
    {
    }

    OddSource_Inline
    MacAddress::
    MacAddress(
        ::std::string_view const & repr )
        : MacAddress( std::string( repr ), fromRepr(repr), predictReprLength(repr) )
    {
    }

    OddSource_Inline
    MacAddress::
    MacAddress(
        ::std::uint8_t const data[MAX_ADAPTER_ADDRESS_LENGTH],
        ::std::uint8_t dataLength )
        : MacAddress( toRepr(data, dataLength), copyHardwareAddress(data, dataLength), dataLength )
    {
    }

    OddSource_Inline
    MacAddress::
    MacAddress(
        ::std::string && repr,
        ::std::unique_ptr< ::std::uint8_t const[] > && data,
        ::std::uint8_t dataLength )
        : _representation( std::move( repr ) ),
          _data( ::std::move( data ) ),
          _data_length( dataLength )
    {
    }

    OddSource_Inline
    MacAddress::
    MacAddress(
        MacAddress const & other )
        : _representation(other._representation),
          _data(copyHardwareAddress(other._data.get(), other._data_length)),
          _data_length(other._data_length)
    {
    }

    OddSource_Inline
    MacAddress::
    MacAddress(
        MacAddress && other ) noexcept
        : _representation( std::move( other._representation ) ),
          _data( std::move( other._data ) ),
          _data_length( other._data_length )
    {
    }

    OddSource_Inline
    MacAddress::
    ~MacAddress() noexcept
    {
    }

    OddSource_Inline
    MacAddress::
    operator ::std::string() const
    {
        return this->_representation;
    }

    OddSource_Inline
    MacAddress::
    operator char const *() const
    {
        return this->_representation.c_str();
    }

    OddSource_Inline
    MacAddress::
    operator ::std::uint8_t const *() const
    {
        return this->_data.get();
    }

    OddSource_Inline
    ::std::uint8_t
    MacAddress::
    length() const
    {
        return this->_data_length;
    }

    OddSource_Inline
    bool
    MacAddress::
    operator==(
        MacAddress const & other ) const
    {
        if (this->_data_length != other._data_length)
        {
            return false;
        }
        for (::std::uint8_t i(0); i < this->_data_length; i++)
        {
            if (this->_data[i] != other._data[i])
            {
                return false;
            }
        }
        return true;
    }

    OddSource_Inline
    bool
    MacAddress::
    operator!=(
        MacAddress const & other ) const
    {
        return !this->operator==(other);
    }

    OddSource_Inline
    ::std::string
    toString(
        MacAddress const & address )
    {
        return address.operator::std::string();
    }

    OddSource_Inline
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        MacAddress const & address )
    {
        return os << address.operator::std::string();
    }
}
