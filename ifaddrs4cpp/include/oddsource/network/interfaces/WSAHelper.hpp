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

#ifndef ODDSOURCE_NETWORK_INTERFACES_WSAHELPER_HPP
#define ODDSOURCE_NETWORK_INTERFACES_WSAHELPER_HPP

#include "detail/config.h"
#include "detail/winsock_includes.h"

#ifdef ODDSOURCE_IS_WINDOWS

namespace OddSource::Interfaces
{
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
}

#ifdef IFADDRS4CPP_INLINE_SOURCE
#include "impl/WSAHelper.ipp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

#endif /* ODDSOURCE_IS_WINDOWS */

#endif /* ODDSOURCE_NETWORK_INTERFACES_WSAHELPER_HPP */
