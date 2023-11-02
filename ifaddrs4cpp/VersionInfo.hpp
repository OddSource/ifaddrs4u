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

#include <string>

inline ::std::string
OddSource::Interfaces::VersionInfo::
version()
{
    if constexpr (std::char_traits<char>::length(IFADDRS4CPP_VERSION_SUFFIX) > 0)
    {
        return ::std::string(IFADDRS4CPP_VERSION) + "-" + IFADDRS4CPP_VERSION_SUFFIX;
    }
    return IFADDRS4CPP_VERSION;
}

inline uint32_t
OddSource::Interfaces::VersionInfo::
major_version()
{
    return IFADDRS4CPP_VERSION_MAJOR;
}

inline uint32_t
OddSource::Interfaces::VersionInfo::
minor_version()
{
    return IFADDRS4CPP_VERSION_MINOR;
}

inline uint32_t
OddSource::Interfaces::VersionInfo::
patch_version()
{
    return IFADDRS4CPP_VERSION_PATCH;
}

inline ::std::optional<::std::string>
OddSource::Interfaces::VersionInfo::
suffix()
{
#include "s.h" // limit the scope of the `using` to this function body
    if constexpr (std::char_traits<char>::length(IFADDRS4CPP_VERSION_SUFFIX) > 0)
    {
        return "-"s + IFADDRS4CPP_VERSION_SUFFIX;
    }
    else
    {
        return ::std::nullopt;
    }
}

inline ::std::string
OddSource::Interfaces::VersionInfo::
git_hash()
{
    return IFADDRS4CPP_GIT_HASH;
}

inline ::std::string
OddSource::Interfaces::VersionInfo::
git_hash_short()
{
    return IFADDRS4CPP_GIT_HASH_SHORT;
}
