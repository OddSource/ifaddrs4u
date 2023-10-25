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
