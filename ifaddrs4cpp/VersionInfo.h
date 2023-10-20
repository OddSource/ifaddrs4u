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
