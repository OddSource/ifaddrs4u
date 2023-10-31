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

#include <optional>
#include <string>

#include "config.h"

namespace OddSource::Interfaces
{
    class OddSource_Export VersionInfo
    {
    public:
        VersionInfo(VersionInfo const &) = delete;

        VersionInfo(VersionInfo &&) = delete;

        static inline ::std::string version();

        static inline uint32_t major_version();

        static inline uint32_t minor_version();

        static inline uint32_t patch_version();

        static inline ::std::optional<::std::string> suffix();

        static inline ::std::string git_hash();

        static inline ::std::string git_hash_short();

    private:
        VersionInfo() {}

        ~VersionInfo() = default;
    };
}

#include "VersionInfo.hpp"
