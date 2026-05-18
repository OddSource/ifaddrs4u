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

#ifndef ODDSOURCE_NETWORK_INTERFACES_VERSIONINFO_HPP
#define ODDSOURCE_NETWORK_INTERFACES_VERSIONINFO_HPP

#include "detail/config.h"

#ifdef ODDSOURCE_IS_WINDOWS
#  pragma warning( push )
#  pragma warning( disable : 4242 )
#  pragma warning( disable : 4244 )
#endif /* ODDSOURCE_IS_WINDOWS */
#include <optional>
#include <string>
#ifdef ODDSOURCE_IS_WINDOWS
#  pragma warning( pop )
#endif /* ODDSOURCE_IS_WINDOWS */

namespace OddSource::Interfaces
{
    class OddSource_Export VersionInfo
    {
    public:
        VersionInfo() = delete;

        [[nodiscard]]
        static
        OddSource_Inline
        ::std::string
        version();

        [[nodiscard]]
        static
        OddSource_Inline
        ::std::uint32_t
        majorVersion();

        [[nodiscard]]
        static
        OddSource_Inline
        ::std::uint32_t
        minorVersion();

        [[nodiscard]]
        static
        OddSource_Inline
        ::std::uint32_t
        patchVersion();

        [[nodiscard]]
        static
        OddSource_Inline
        ::std::optional< ::std::string >
        suffix();

        [[nodiscard]]
        static
        OddSource_Inline
        ::std::string
        gitHash();

        [[nodiscard]]
        static
        OddSource_Inline
        ::std::string
        gitHashShort();
    };
}

#ifdef IFADDRS4CPP_INLINE_SOURCE
#include "impl/VersionInfo.ipp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

#endif /* ODDSOURCE_NETWORK_INTERFACES_VERSIONINFO_HPP */
