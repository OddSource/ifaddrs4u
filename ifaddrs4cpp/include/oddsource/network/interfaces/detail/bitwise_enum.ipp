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

#ifndef ODDSOURCE_NETWORK_INTERFACES_DETAIL_BITWISE_ENUM_IPP
#define ODDSOURCE_NETWORK_INTERFACES_DETAIL_BITWISE_ENUM_IPP

#include "config.h"

#include <type_traits>
// ReSharper disable once CppUnusedIncludeDirective
#include <utility>

namespace OddSource::Interfaces::detail
{
    template< class Enum >
#if __cplusplus >= 202302L
    using to_underlying = ::std::to_underlying< Enum >;
#else
    constexpr
    ::std::underlying_type_t< Enum >
    to_underlying(
        Enum e ) noexcept
    {
        return static_cast< ::std::underlying_type_t< Enum > >( e );
    }

    template< class Enum, typename result >
    constexpr
    ::std::underlying_type_t< Enum >
    recast_underlying(
        result r ) noexcept
    {
        return static_cast< ::std::underlying_type_t< Enum > >( r );
    }
#endif
}

// NOTE: All of the recasts of the return values below are to appease MSVC, which "upgrades" bitwise results of
// ushorts and uints to ints.

#define ODDSOURCE_BITWISE_ENUM_OPERATORS_IMPL(Enum) \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > \
    operator&( \
        Enum const & lhs, \
        Enum const & rhs ) noexcept \
    { \
        return detail::recast_underlying< Enum >( \
            detail::to_underlying< Enum >( lhs ) & detail::to_underlying< Enum >( rhs ) ); \
    } \
    \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > \
    operator&( \
        ::std::underlying_type_t< Enum > const lhs, \
        Enum const & rhs ) noexcept \
    { \
        return detail::recast_underlying< Enum >( lhs & detail::to_underlying< Enum >( rhs ) ); \
    } \
    \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > \
    operator&( \
        Enum const & lhs, \
        ::std::underlying_type_t< Enum > const rhs ) noexcept \
    { \
        return detail::recast_underlying< Enum >( detail::to_underlying< Enum >( lhs ) & rhs ); \
    } \
    \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > & \
    operator&=( \
        ::std::underlying_type_t< Enum > & lhs, \
        Enum const & rhs ) noexcept \
    { \
        lhs &= detail::to_underlying< Enum >( rhs ); \
        return lhs; \
    } \
    \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > \
    operator|( \
        Enum const & lhs, \
        Enum const & rhs ) noexcept \
    { \
        return detail::recast_underlying< Enum >( \
            detail::to_underlying< Enum >( lhs ) | detail::to_underlying< Enum >( rhs ) ); \
    } \
    \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > \
    operator|( \
        ::std::underlying_type_t< Enum > const lhs, \
        Enum const & rhs ) noexcept \
    { \
        return detail::recast_underlying< Enum >( lhs | detail::to_underlying< Enum >( rhs ) ); \
    } \
    \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > \
    operator|( \
        Enum const & lhs, \
        ::std::underlying_type_t< Enum > const rhs ) noexcept \
    { \
        return detail::recast_underlying< Enum >( detail::to_underlying< Enum >( lhs ) | rhs ); \
    } \
    \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > & \
    operator|=( \
        ::std::underlying_type_t< Enum > & lhs, \
        Enum const & rhs ) noexcept \
    { \
        lhs |= detail::to_underlying< Enum >( rhs ); \
        return lhs; \
    } \
    \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > \
    operator^( \
        Enum const & lhs, \
        Enum const & rhs ) noexcept \
    { \
        return detail::recast_underlying< Enum >( \
            detail::to_underlying< Enum >( lhs ) ^ detail::to_underlying< Enum >( rhs ) ); \
    } \
    \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > \
    operator^( \
        ::std::underlying_type_t< Enum > const lhs, \
        Enum const & rhs ) noexcept \
    { \
        return detail::recast_underlying< Enum >( lhs ^ detail::to_underlying< Enum >( rhs ) ); \
    } \
    \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > \
    operator^( \
        Enum const & lhs, \
        ::std::underlying_type_t< Enum > const rhs ) noexcept \
    { \
        return detail::recast_underlying< Enum >( detail::to_underlying< Enum >( lhs ) ^ rhs ); \
    } \
    \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > & \
    operator^=( \
        ::std::underlying_type_t< Enum > & lhs, \
        Enum const & rhs ) noexcept \
    { \
        lhs ^= detail::to_underlying< Enum >( rhs ); \
        return lhs; \
    } \
    \
    OddSource_Inline \
    ::std::underlying_type_t< Enum > \
    operator~( \
        Enum const & rhs ) noexcept \
    { \
        /* Have to cast because MSVC promotes ~ushort and ~uint to an int, resulting in an error on return */ \
        return detail::recast_underlying< Enum >( ~detail::to_underlying< Enum >( rhs ) ); \
    } \
    \
    OddSource_Inline \
    bool \
    operator==( \
        ::std::underlying_type_t< Enum > const lhs, \
        Enum const & rhs ) noexcept \
    { \
        return lhs == detail::to_underlying< Enum >( rhs ); \
    } \
    \
    OddSource_Inline \
    bool \
    operator==( \
        Enum const & lhs, \
        ::std::underlying_type_t< Enum > const rhs ) noexcept \
    { \
        return detail::to_underlying< Enum >( lhs ) == rhs; \
    } \
    \
    OddSource_Inline \
    bool \
    operator!=( \
        ::std::underlying_type_t< Enum > const lhs, \
        Enum const & rhs ) noexcept \
    { \
        return lhs != detail::to_underlying< Enum >( rhs ); \
    } \
    \
    OddSource_Inline \
    bool \
    operator!=( \
        Enum const & lhs, \
        ::std::underlying_type_t< Enum > const rhs ) noexcept \
    { \
        return detail::to_underlying< Enum >( lhs ) != rhs; \
    }

#endif /* ODDSOURCE_NETWORK_INTERFACES_DETAIL_BITWISE_ENUM_IPP */
