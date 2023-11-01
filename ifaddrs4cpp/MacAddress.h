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

#pragma once

#include "config.h"

#include <iostream>
#include <memory>
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
        explicit InvalidMacAddress(::std::string_view const & what)
                : ::std::invalid_argument(::std::string(what)) {}
        InvalidMacAddress(InvalidMacAddress const & other) noexcept = default;
    };

    struct MacTempDataHolder;

    class OddSource_Export MacAddress
    {
    public:
        MacAddress() = delete;

        // copy constructor
        MacAddress(MacAddress const &);

        // move constructor
        MacAddress(MacAddress &&) noexcept = default;

        MacAddress(::std::string_view const &); // NOLINT(*-explicit-constructor)

        MacAddress(uint8_t const [MAX_ADAPTER_ADDRESS_LENGTH], uint8_t);

        ~MacAddress() = default;

        [[nodiscard]]
        inline operator ::std::string() const; // NOLINT(*-explicit-constructor)

        [[nodiscard]]
        inline operator char const *() const; // NOLINT(*-explicit-constructor)

        [[nodiscard]]
        inline operator uint8_t const *() const; // NOLINT(*-explicit-constructor)

        [[nodiscard]]
        inline uint8_t length() const;

        [[nodiscard]]
        inline bool operator==(MacAddress const &) const;

        [[nodiscard]]
        inline bool operator!=(MacAddress const &) const;

    private:
        MacAddress(::std::string_view const &, MacTempDataHolder const &);

        ::std::string const _representation;
        ::std::unique_ptr<uint8_t const[]> _data;
        uint8_t const _data_length;
    };

    inline ::std::ostream & operator<<(::std::ostream &, MacAddress const &);
}

#include "MacAddress.hpp"
