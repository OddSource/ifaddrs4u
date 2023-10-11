#include "../Interfaces.h"
#include "main.h"

using namespace OddSource::Interfaces;

class TestVersionInfo : public Tests::Test
{
public:
    TestVersionInfo()
        : Test()
    {
        add_test(test_version_info);
    }

    void test_version_info()
    {
        assert_equals(VersionInfo::version(), IFADDRS4CPP_VERSION);
        assert_equals(VersionInfo::major_version(), IFADDRS4CPP_VERSION_MAJOR);
        assert_equals(VersionInfo::minor_version(), IFADDRS4CPP_VERSION_MINOR);
        assert_equals(VersionInfo::patch_version(), IFADDRS4CPP_VERSION_PATCH);

        if (strlen(IFADDRS4CPP_VERSION_SUFFIX) > 0)
        {
            ::std::string suffix("-");
            suffix += IFADDRS4CPP_VERSION_SUFFIX;
            assert_equals(*VersionInfo::suffix(), suffix);
        }
        else
        {
            assert_not_that((bool)VersionInfo::suffix());
        }

        assert_equals(VersionInfo::git_hash(), IFADDRS4CPP_GIT_HASH);
        assert_equals(VersionInfo::git_hash_short(), IFADDRS4CPP_GIT_HASH_SHORT);
    }

    static std::unique_ptr<Test> create()
    {
        return std::make_unique<TestVersionInfo>();
    }
};

namespace
{
    [[maybe_unused]]
    Tests::Test::Registrar<TestVersionInfo> registrar("TestVersionInfo");
}
