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

#ifndef ODDSOURCE_NETWORK_INTERFACES_DETAIL_BITWISE_ENUM_H
#define ODDSOURCE_NETWORK_INTERFACES_DETAIL_BITWISE_ENUM_H

#include "config.h"

#include <type_traits>

#define ODDSOURCE_BITWISE_ENUM_OPERATORS_DECL(Enum) \
    [[nodiscard]] \
    OddSource_Export \
    ::std::underlying_type_t< Enum > \
    operator&( \
        Enum const & lhs, \
        Enum const & rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    ::std::underlying_type_t< Enum > \
    operator&( \
        ::std::underlying_type_t< Enum > lhs, \
        Enum const & rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    ::std::underlying_type_t< Enum > \
    operator&( \
        Enum const & lhs, \
        ::std::underlying_type_t< Enum > rhs ) noexcept; \
    \
    OddSource_Export \
    ::std::underlying_type_t< Enum > & \
    operator&=( \
        ::std::underlying_type_t< Enum > & lhs, \
        Enum const & rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    ::std::underlying_type_t< Enum > \
    operator|( \
        Enum const & lhs, \
        Enum const & rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    ::std::underlying_type_t< Enum > \
    operator|( \
        ::std::underlying_type_t< Enum > lhs, \
        Enum const & rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    ::std::underlying_type_t< Enum > \
    operator|( \
        Enum const & lhs, \
        ::std::underlying_type_t< Enum > rhs ) noexcept; \
    \
    OddSource_Export \
    ::std::underlying_type_t< Enum > & \
    operator|=( \
        ::std::underlying_type_t< Enum > & lhs, \
        Enum const & rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    ::std::underlying_type_t< Enum > \
    operator^( \
        Enum const & lhs, \
        Enum const & rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    ::std::underlying_type_t< Enum > \
    operator^( \
        ::std::underlying_type_t< Enum > lhs, \
        Enum const & rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    ::std::underlying_type_t< Enum > \
    operator^( \
        Enum const & lhs, \
        ::std::underlying_type_t< Enum > rhs ) noexcept; \
    \
    OddSource_Export \
    ::std::underlying_type_t< Enum > & \
    operator^=( \
        ::std::underlying_type_t< Enum > & lhs, \
        Enum const & rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    ::std::underlying_type_t< Enum > \
    operator~( \
        Enum const & rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    bool \
    operator==( \
        ::std::underlying_type_t< Enum > lhs, \
        Enum const & rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    bool \
    operator==( \
        Enum const & lhs, \
        ::std::underlying_type_t< Enum > rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    bool \
    operator!=( \
        ::std::underlying_type_t< Enum > lhs, \
        Enum const & rhs ) noexcept; \
    \
    [[nodiscard]] \
    OddSource_Export \
    bool \
    operator!=( \
        Enum const & lhs, \
        ::std::underlying_type_t< Enum > rhs ) noexcept

#endif /* ODDSOURCE_NETWORK_INTERFACES_DETAIL_BITWISE_ENUM_H */
