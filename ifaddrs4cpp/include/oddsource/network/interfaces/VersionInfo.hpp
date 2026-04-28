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

#ifndef ODDSOURCE_NETWORK_INTERFACES_VERSIONINFO_HPP
#define ODDSOURCE_NETWORK_INTERFACES_VERSIONINFO_HPP

#include "detail/config.h"

#include <optional>
#include <string>

namespace OddSource::Interfaces
{
    class OddSource_Export VersionInfo
    {
    public:
        VersionInfo() = delete;

        static
        OddSource_Inline
        ::std::string
        version();

        static
        OddSource_Inline
        ::std::uint32_t
        major_version();

        static
        OddSource_Inline
        ::std::uint32_t
        minor_version();

        static
        OddSource_Inline
        ::std::uint32_t
        patch_version();

        static
        OddSource_Inline
        ::std::optional< ::std::string >
        suffix();

        static
        OddSource_Inline
        ::std::string
        git_hash();

        static
        OddSource_Inline
        ::std::string
        git_hash_short();
    };
}

#ifdef IFADDRS4CPP_INLINE_SOURCE
#include "impl/VersionInfo.ipp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

#endif /* ODDSOURCE_NETWORK_INTERFACES_VERSIONINFO_HPP */
