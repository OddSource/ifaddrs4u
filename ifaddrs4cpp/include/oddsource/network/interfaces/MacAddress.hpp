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

#ifndef ODDSOURCE_NETWORK_INTERFACES_MACADDRESS_HPP
#define ODDSOURCE_NETWORK_INTERFACES_MACADDRESS_HPP

#include "detail/config.h"

#ifdef ODDSOURCE_IS_WINDOWS
#  pragma warning( push )
#  pragma warning( disable : 4242 )
#  pragma warning( disable : 4244 )
#  pragma warning( disable : 4710 )
#  pragma warning( disable : 4711 )
#endif /* ODDSOURCE_IS_WINDOWS */
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#ifdef ODDSOURCE_IS_WINDOWS
#  pragma warning( pop )
#endif /* ODDSOURCE_IS_WINDOWS */

namespace OddSource::Interfaces
{
    class OddSource_Export InvalidMacAddress : public ::std::invalid_argument
    {
    public:
        OddSource_Inline
        explicit
        InvalidMacAddress(
            ::std::string_view const & what );

        OddSource_Inline
        InvalidMacAddress(
            InvalidMacAddress const & other );

        OddSource_Inline
        virtual
        ~InvalidMacAddress() noexcept; // NOLINT(*-use-override)
    };

    class OddSource_Export MacAddress
    {
    public:
        static constexpr ::std::uint16_t MIN_LENGTH{ 6 };
        static constexpr ::std::uint16_t MAX_LENGTH{ 8 };

        MacAddress() = delete;

        OddSource_Inline
        explicit
        MacAddress(
            ::std::string_view const & repr );

        OddSource_Inline
        MacAddress(
            ::std::uint8_t const data[ MAX_LENGTH ],
            ::std::uint16_t dataLength );

        OddSource_Inline
        MacAddress(
            MacAddress const & other );

        OddSource_Inline
        MacAddress(
            MacAddress && other ) noexcept;

        virtual
        OddSource_Inline
        ~MacAddress() noexcept;

        OddSource_Inline
        MacAddress &
        operator=(
            MacAddress const & rhs );

        OddSource_Inline
        MacAddress &
        operator=(
            MacAddress && rhs ) noexcept;

        [[nodiscard]]
        OddSource_Inline
        explicit
        operator ::std::string() const;

        [[nodiscard]]
        OddSource_Inline
        explicit
        operator char const *() const;

        [[nodiscard]]
        OddSource_Inline
        explicit
        operator ::std::uint8_t const *() const;

        [[nodiscard]]
        OddSource_Inline
        ::std::uint16_t
        length() const;

        [[nodiscard]]
        OddSource_Inline
        bool
        operator==(
            MacAddress const & rhs ) const;

        [[nodiscard]]
        OddSource_Inline
        bool
        operator!=(
            MacAddress const & rhs ) const;

    private:
        OddSource_Inline
        MacAddress(
            ::std::string && repr,
            ::std::unique_ptr< ::std::uint8_t const[] > && data,
            ::std::uint16_t dataLength );

        ::std::string _representation;
        ::std::unique_ptr< ::std::uint8_t const[] > _data;
        ::std::uint16_t _dataLength;
    };

    [[nodiscard]]
    OddSource_Export
    ::std::string
    toString(
        MacAddress const & address );

    OddSource_Export
    ::std::ostream &
    operator<<(
        ::std::ostream & os,
        MacAddress const & address );
}

#ifdef IFADDRS4CPP_INLINE_SOURCE
#include "impl/MacAddress.ipp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

#endif /* ODDSOURCE_NETWORK_INTERFACES_MACADDRESS_HPP */
