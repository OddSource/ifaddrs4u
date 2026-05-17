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

#ifdef ODDSOURCE_IS_WINDOWS
#  pragma warning( push )
#  pragma warning( disable : 4242 )
#  pragma warning( disable : 4244 )
#  pragma warning( disable : 4710 )
#endif /* ODDSOURCE_IS_WINDOWS */
#include <algorithm>
// ReSharper disable once CppUnusedIncludeDirective
#include <cctype>
// ReSharper disable once CppUnusedIncludeDirective
#include <cstdio>
#include <cstring>
#include <memory>
#include <sstream>
#include <utility>
#ifdef ODDSOURCE_IS_WINDOWS
#  pragma warning( pop )
#endif /* ODDSOURCE_IS_WINDOWS */

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace
{
    using namespace OddSource::Interfaces;

    constexpr ::std::uint16_t MAC_ADDRESS_CLAMP_MIN{ 0 };
    constexpr ::std::uint16_t MAC_ADDRESS_CLAMP_MAX{ 256 };

    ::std::unique_ptr< ::std::uint8_t[] >
    copyHardwareAddress(
        ::std::uint8_t const data[ MacAddress::MAX_LENGTH ],
        ::std::uint16_t const dataLength )
    {
        auto newData( ::std::make_unique< ::std::uint8_t[] >( dataLength ) );
        ::std::memcpy( newData.get(), data, dataLength );
        return newData;
    }

    ::std::uint16_t
    predictReprLength(
        ::std::string_view const & repr )
    {
        ::std::uint16_t count{ 0 };
        for ( char const & c: repr )
        {
            if ( c == ':' || c == '-' )
            {
                count++;
            }
        }
        return count + 1u;
    }

#define MAC_ADDR_REPR_POS ::std::string( repr ) + "' at position "s + ::std::to_string( position )

    ::std::unique_ptr< ::std::uint8_t[] >
    fromRepr(
        ::std::string_view const & repr )
    {
        using namespace ::std::string_literals;
        // GNUC has ether_aton_r, which is thread-safe, but BSD systems have
        // ether_aton, which is not thread safe and basically cannot safely be used.
        // Windows has no built-in method until C#. So ... let's try something simple-ish.
        auto const predictedLength( static_cast< size_t >(
            ::std::clamp( predictReprLength( repr ), MAC_ADDRESS_CLAMP_MIN, MAC_ADDRESS_CLAMP_MAX ) ) );
        if ( predictedLength > MacAddress::MAX_LENGTH )
        {
            ::std::ostringstream oss;
            oss << "MAC address length (" << predictedLength
                << " bytes) too long (max " << MacAddress::MAX_LENGTH
                << " bytes).";
            throw InvalidMacAddress( oss.str() );
        }
        if ( predictedLength < MacAddress::MIN_LENGTH )
        {
            ::std::ostringstream oss;
            oss << "MAC address length (" << predictedLength
                 << " bytes) too short (min " << MacAddress::MIN_LENGTH
                 << " bytes).";
            throw InvalidMacAddress( oss.str() );
        }

        auto data( ::std::make_unique< ::std::uint8_t[] >( MacAddress::MAX_LENGTH ) );
        size_t size{ 0 }, position{ 0 };
        ::std::uint8_t byte{ 0 }, charsInByte{ 0 };
        for ( char c : repr )
        {
            if ( auto const ch{ static_cast< char >( ::std::tolower( c ) ) }; ch == ':' || ch == '-' )
            {
                if ( charsInByte != 2 )
                {
                    throw InvalidMacAddress( "Unexpected separator in MAC address '"s + MAC_ADDR_REPR_POS );
                }
                data[ size++ ] = byte;
                byte = charsInByte = 0;
            }
            else if ( ( ch >= '0' && ch <= '9' ) || ( ch >= 'a' && ch <= 'f' ) )
            {
                charsInByte++;
                if ( charsInByte > 2 )
                {
                    auto const invalidNumChars{ ::std::clamp(
                        static_cast< ::std::uint16_t >( charsInByte ), MAC_ADDRESS_CLAMP_MIN, MAC_ADDRESS_CLAMP_MAX ) };
                    throw InvalidMacAddress(
                        "Invalid number of characters "s + ::std::to_string( invalidNumChars ) +
                        " between separators in MAC address '"s + MAC_ADDR_REPR_POS );
                }
                byte <<= 4;
                byte += ::std::isdigit( ch ) ? ch - '0' : ch - 'a' + 10;
            }
            else
            {
                throw InvalidMacAddress( "Invalid character '"s + ch + "' in MAC address "s + MAC_ADDR_REPR_POS );
            }

            position++;
        }
        if ( charsInByte != 2 )
        {
            throw InvalidMacAddress( "Unexpected separator in MAC address '"s + MAC_ADDR_REPR_POS );
        }
        data[ size++ ] = byte;
        if( size != predictedLength )
        {
            throw InvalidMacAddress(
                "Malformed MAC address '"s + ::std::string( repr ) + "' did not match expected length "s +
                ::std::to_string( predictedLength ) + "."s);
        }

        return data;
    }

#undef MAC_ADDR_REPR_POS

    ::std::string
    toRepr(
        ::std::uint8_t const data[ MacAddress::MAX_LENGTH ],
        ::std::uint16_t const dataLength )
    {
        ::std::ostringstream oss;
        auto const clampedLength{ ::std::clamp( dataLength, MAC_ADDRESS_CLAMP_MIN, MAC_ADDRESS_CLAMP_MAX ) };
        if ( clampedLength > MacAddress::MAX_LENGTH )
        {
            oss << "MAC address length (" << clampedLength
                << ") greater than allowed length " << MacAddress::MAX_LENGTH;
            throw InvalidMacAddress( oss.str() );
        }
        if ( clampedLength < MacAddress::MIN_LENGTH )
        {
            oss << "MAC address length (" << clampedLength
                << " bytes) too short (min " << MacAddress::MIN_LENGTH << " bytes).";
            throw InvalidMacAddress( oss.str() );
        }

        static char const HEX_DIGITS[] = "0123456789abcdef";
        for( ::std::uint16_t i{ 0 }; i < clampedLength; ++i )
        {
            if ( i > 0 )
            {
                oss << ':';
            }
            ::std::uint8_t const byte{ data[ i ] };
            oss << HEX_DIGITS[ byte >> 4 ] << HEX_DIGITS[ byte & 0x0F ];
        }

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
    InvalidMacAddress( // NOLINT(*-use-equals-default)
        InvalidMacAddress const & other )
        : ::std::invalid_argument( other )
    {
    }

    OddSource_Inline
    InvalidMacAddress::
    ~InvalidMacAddress() noexcept // NOLINT(*-use-equals-default)
    {
    }

    OddSource_Inline
    MacAddress::
    MacAddress(
        ::std::string_view const & repr )
        : MacAddress( ::std::string( repr ), fromRepr( repr ), predictReprLength( repr ) )
    {
    }

    OddSource_Inline
    MacAddress::
    MacAddress(
        ::std::uint8_t const data[ MAX_LENGTH ],
        ::std::uint16_t const dataLength )
        : MacAddress( toRepr( data, dataLength ), copyHardwareAddress( data, dataLength ), dataLength )
    {
    }

    OddSource_Inline
    MacAddress::
    MacAddress(
        ::std::string && repr,
        ::std::unique_ptr< ::std::uint8_t const[] > && data,
        ::std::uint16_t const dataLength )
        : _representation( ::std::move( repr ) ),
          _data( ::std::move( data ) ),
          _dataLength( dataLength )
    {
    }

    OddSource_Inline
    MacAddress::
    MacAddress(
        MacAddress const & other )
        : _representation( other._representation ),
          _data( copyHardwareAddress( other._data.get(), other._dataLength ) ),
          _dataLength( other._dataLength )
    {
    }

    OddSource_Inline
    MacAddress::
    MacAddress(
        MacAddress && other ) noexcept
        : _representation( ::std::move( other._representation ) ),
          _data( ::std::move( other._data ) ),
          _dataLength( other._dataLength )
    {
        other._dataLength = 0;
    }

    OddSource_Inline
    MacAddress::
    ~MacAddress() noexcept // NOLINT(*-use-equals-default)
    {
    }

    OddSource_Inline
    MacAddress &
    MacAddress::
    operator=(
        MacAddress const & rhs )
    {
        if ( this != &rhs )
        {
            this->_representation = rhs._representation;
            this->_data = copyHardwareAddress( rhs._data.get(), rhs._dataLength );
            this->_dataLength = rhs._dataLength;
        }
        return *this;
    }

    OddSource_Inline
    MacAddress &
    MacAddress::
    operator=(
        MacAddress && rhs ) noexcept
    {
        this->_representation = ::std::move( rhs._representation );
        this->_data = ::std::move( rhs._data );
        this->_dataLength = rhs._dataLength;
        rhs._dataLength = 0;
        return *this;
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
    ::std::uint16_t
    MacAddress::
    length() const
    {
        return this->_dataLength;
    }

    OddSource_Inline
    bool
    MacAddress::
    operator==(
        MacAddress const & other ) const
    {
        if ( this == &other )
        {
            return true;
        }
        if ( this->_dataLength != other._dataLength )
        {
            return false;
        }
        for ( ::std::uint16_t i{ 0 }; i < this->_dataLength; i++ )
        {
            if ( this->_data[ i ] != other._data[ i ] )
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
        return !this->operator==( other );
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
