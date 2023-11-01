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

#include "config.h"

#include <functional>
#include <memory>
#include <shared_mutex>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "Interface.h"

namespace OddSource::Interfaces
{
    class OddSource_Export InterfaceBrowserSystemError : public ::std::runtime_error
    {
    public:
        explicit InterfaceBrowserSystemError(std::string_view const & what)
            : ::std::runtime_error(::std::string(what)) {};
        InterfaceBrowserSystemError(InterfaceBrowserSystemError const & other) noexcept = default;
    };

    class OddSource_Export WinSockStartupCleanupHelper
    {
    public:
        WinSockStartupCleanupHelper();

        WinSockStartupCleanupHelper(WinSockStartupCleanupHelper const &) = delete;

        WinSockStartupCleanupHelper(WinSockStartupCleanupHelper &&) = delete;

        ~WinSockStartupCleanupHelper();
    };

    class OddSource_Export InterfaceBrowser
    {
    public:
        InterfaceBrowser();

        InterfaceBrowser(InterfaceBrowser const &) = delete;

        InterfaceBrowser &
        operator=(InterfaceBrowser const &) = delete;

        bool for_each_interface(::std::function<bool(Interface const &)> & do_this);

        ::std::vector<Interface> const & get_interfaces();

        Interface const & get_interface(uint32_t);

        Interface const & get_interface(::std::string_view const &);

    private:
        void populate_interface_storage();

        bool populate_and_maybe_more_unsafe(::std::function<bool(Interface const &)> * do_this);

        mutable std::shared_mutex _storage_mutex;
        bool _storage_filled;
        ::std::vector<Interface> _interface_vector;
        ::std::unordered_map<uint32_t, ::std::shared_ptr<Interface const>> _index_map;
        ::std::unordered_map<::std::string, ::std::shared_ptr<Interface const>> _name_map;

        static WinSockStartupCleanupHelper const _wsa_helper;
    };
}
