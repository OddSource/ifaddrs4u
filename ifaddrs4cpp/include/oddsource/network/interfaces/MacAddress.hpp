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

#include <memory>
#include <ostream>
#include <string>
#include <string_view>

#ifndef MIN_ADAPTER_ADDRESS_LENGTH
#define MIN_ADAPTER_ADDRESS_LENGTH 6
#endif

#ifndef MAX_ADAPTER_ADDRESS_LENGTH
#define MAX_ADAPTER_ADDRESS_LENGTH 8
#endif /* MAX_ADAPTER_ADDRESS_LENGTH */

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

        virtual
        OddSource_Inline
        ~InvalidMacAddress() noexcept;
    };

    class OddSource_Export MacAddress
    {
    public:
        MacAddress() = delete;

        OddSource_Inline
        explicit
        MacAddress(
            ::std::string_view const & repr );

        OddSource_Inline
        MacAddress(
            ::std::uint8_t const data[ MAX_ADAPTER_ADDRESS_LENGTH ],
            ::std::uint8_t dataLength );

        OddSource_Inline
        MacAddress(
            MacAddress const & other );

        OddSource_Inline
        MacAddress(
            MacAddress && other ) noexcept;

        virtual
        OddSource_Inline
        ~MacAddress() noexcept;

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
        ::std::uint8_t
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
            ::std::uint8_t dataLength );

        ::std::string const _representation;
        ::std::unique_ptr< ::std::uint8_t const[] > _data;
        ::std::uint8_t const _data_length;
    };

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
