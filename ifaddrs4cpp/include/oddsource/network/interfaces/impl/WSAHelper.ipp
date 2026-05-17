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

#ifndef IFADDRS4CPP_INLINE_SOURCE
#include "../WSAHelper.hpp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

#ifdef ODDSOURCE_IS_WINDOWS

namespace OddSource::Interfaces
{
    OddSource_Inline
    WinSockStartupCleanupHelper::
    WinSockStartupCleanupHelper()
    {
        using namespace ::std::string_literals;
        WORD version_requested = MAKEWORD( 2, 2 );
        WSADATA data;
        int error = WSAStartup( version_requested, &data );
        if (error != 0)
        {
            throw ::std::runtime_error(
                "Could not initialize WinSock subsystem due to error code: "s +
                ::std::to_string(error) + ". For the meaning of this, see the documentation: "s +
                "https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup#return-value"s);
        }
    }

    OddSource_Inline
    WinSockStartupCleanupHelper::
    ~WinSockStartupCleanupHelper()
    {
        WSACleanup();
    }
}

#endif /* ODDSOURCE_IS_WINDOWS */
