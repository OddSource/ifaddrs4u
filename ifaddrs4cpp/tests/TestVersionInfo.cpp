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

#include "../VersionInfo.h"
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
        assert_equals(VersionInfo::major_version(), IFADDRS4CPP_VERSION_MAJOR);
        assert_equals(VersionInfo::minor_version(), IFADDRS4CPP_VERSION_MINOR);
        assert_equals(VersionInfo::patch_version(), IFADDRS4CPP_VERSION_PATCH);

        if (strlen(IFADDRS4CPP_VERSION_SUFFIX) > 0)
        {
            ::std::string suffix("-");
            suffix += IFADDRS4CPP_VERSION_SUFFIX;
            assert_equals(VersionInfo::version(), ::std::string(IFADDRS4CPP_VERSION) + suffix);
            assert_equals(*VersionInfo::suffix(), suffix);
        }
        else
        {
            assert_equals(VersionInfo::version(), IFADDRS4CPP_VERSION);
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
