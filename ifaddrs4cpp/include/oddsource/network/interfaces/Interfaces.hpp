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

#ifndef ODDSOURCE_NETWORK_INTERFACES_INTERFACES_HPP
#define ODDSOURCE_NETWORK_INTERFACES_INTERFACES_HPP

#include "detail/config.h"
#include "Interface.hpp"

#include <functional>
#include <list>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

namespace OddSource::Interfaces
{
    class OddSource_Export InterfaceBrowserSystemError : public ::std::runtime_error
    {
    public:
        OddSource_Inline
        explicit
        InterfaceBrowserSystemError(
            ::std::string_view const & what );

        OddSource_Inline
        InterfaceBrowserSystemError(
            InterfaceBrowserSystemError const & other ) noexcept;

        OddSource_Inline
        virtual
        ~InterfaceBrowserSystemError() noexcept; // NOLINT(*-use-override)
    };

#ifdef ODDSOURCE_IS_WINDOWS
    class OddSource_Export WinSockStartupCleanupHelper final
    {
    public:
        OddSource_Inline
        WinSockStartupCleanupHelper();

        WinSockStartupCleanupHelper(
            WinSockStartupCleanupHelper const & ) = delete;

        OddSource_Inline
        ~WinSockStartupCleanupHelper() noexcept;

        WinSockStartupCleanupHelper &
        operator=(
            WinSockStartupCleanupHelper const & ) = delete;
    };
#endif

    class OddSource_Export InterfaceBrowser final
    {
    public:
        OddSource_Inline
        InterfaceBrowser();

        InterfaceBrowser(
            InterfaceBrowser const & ) = delete;

        OddSource_Inline
        ~InterfaceBrowser() noexcept;

        InterfaceBrowser &
        operator=(
            InterfaceBrowser const & ) = delete;

        OddSource_Inline
        bool
        for_each_interface(
            ::std::function< bool( Interface const & ) > doThis ) const;

        OddSource_Inline
        ::std::list< ::std::shared_ptr< Interface const > > const &
        get_interfaces() const;

        OddSource_Inline
        ::std::shared_ptr< Interface const >
        get_interface(
            ::std::uint32_t index ) const;

        OddSource_Inline
        Interface const &
        operator[](
            ::std::uint32_t index ) const;

        OddSource_Inline
        ::std::shared_ptr< Interface const >
        get_interface(
            ::std::string_view name ) const;

        OddSource_Inline
        Interface const &
        operator[](
            ::std::string_view name ) const;

        OddSource_Inline
        ::std::list< ::std::string > const &
        getWarnings() const;

    private:
        ::std::list< ::std::shared_ptr< Interface const > > _interfaces;
        ::std::unordered_map< ::std::uint32_t, ::std::shared_ptr< Interface const > > _indexToInterface;
        ::std::unordered_map< ::std::string, ::std::shared_ptr< Interface const > > _nameToInterface;
        ::std::list< ::std::string > _warnings;
    };
}

#ifdef IFADDRS4CPP_INLINE_SOURCE
#include "impl/Interfaces.ipp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

#endif /* ODDSOURCE_NETWORK_INTERFACES_INTERFACES_HPP */
