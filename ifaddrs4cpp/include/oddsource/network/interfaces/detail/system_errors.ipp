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
#include "system_errors.hpp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

#ifndef ODDSOURCE_IS_WINDOWS
#include <cerrno>
#include <cstring>
#endif /* !ODDSOURCE_IS_WINDOWS */

#ifdef ODDSOURCE_IS_WINDOWS
#  pragma warning( push )
#  pragma warning( disable : 4242 )
#  pragma warning( disable : 4244 )
#endif /* ODDSOURCE_IS_WINDOWS */
#include <stdexcept>
#ifdef ODDSOURCE_IS_WINDOWS
#  pragma warning( pop )
#endif

namespace OddSource::Interfaces::detail
{
    OddSource_Inline
    ErrorCode_t
    getLastSystemErrorCode(
        [[maybe_unused]] bool const socketFirst /* = false */ )
    {
        ErrorCode_t lastErrorCode{ 0 };
#ifdef ODDSOURCE_IS_WINDOWS

        using namespace ::std::string_literals;

        bool checkedGLE{ false }, checkedWSA{ false };

        while ( lastErrorCode == 0 && !checkedGLE && !checkedWSA )
        {
            // If you reverse-engineer the Windows DLLs, ::WSAGetLastError() merely calls ::GetLastError(). However,
            // the official documentation says they are different / unrelated. As such, it's safest to check them both,
            // in case the implementation changes someday.
            if ( socketFirst || checkedGLE )
            {
                checkedWSA = true;
                auto wsaLastErrorCode{ ::WSAGetLastError() };
                if ( wsaLastErrorCode < 0 )
                {
                    // per the MSVC documentation, this should be impossible
                    throw ::std::runtime_error(
                        "::WSAGetLastError() unexpectedly returned negative value "s +
                        ::std::to_string( wsaLastErrorCode ) + "incompatible with the ::FormatMessage interface." );
                }
                lastErrorCode = static_cast< ErrorCode_t >( wsaLastErrorCode );
            }

            if ( lastErrorCode == 0 )
            {
                checkedGLE = true;
                lastErrorCode = ::GetLastError();
            }
        }

        if ( lastErrorCode != 0 )
        {
            ::WSASetLastError( 0 );
            ::SetLastError( 0 ) ;
        }

#else /* ODDSOURCE_IS_WINDOWS */

        lastErrorCode = errno;
        if ( lastErrorCode != 0 )
        {
            errno = 0;
        }

#endif /* !ODDSOURCE_IS_WINDOWS */

        return lastErrorCode;
    }

    OddSource_Inline
    ::std::string
    getSystemErrorMessage(
        ErrorCode_t const errorCode )
    {
        using namespace ::std::string_literals;
        ::std::string errorMessage;

#ifdef ODDSOURCE_IS_WINDOWS

        char * errorMessageBuffer( nullptr );
        DWORD const result( ::FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            errorCode,
            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
            ( LPTSTR )&errorMessageBuffer,
            0,
            nullptr ) );

        if ( result > 0 )
        {
            errorMessage = errorMessageBuffer;
        }

        ::LocalFree( errorMessageBuffer );

#else /* ODDSOURCE_IS_WINDOWS */

#  if defined( __clang__ ) && ODDSOURCE_IS_MACOS
#    define STRERROR_R_RETURNS_INT 1
#  elif ( _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600 ) && !_GNU_SOURCE
#    define STRERROR_R_RETURNS_INT 1
#  else
#    define STRERROR_R_RETURNS_INT 0
#  endif

        static constexpr size_t MAX_LENGTH{ 4096 };
        char errorMessageBuffer[ MAX_LENGTH ];
#if STRERROR_R_RETURNS_INT == 1
        if ( ::strerror_r( errorCode, errorMessageBuffer, MAX_LENGTH ) == 0 )
        {
            errorMessage = errorMessageBuffer;
        }
#else
        errorMessage = ::strerror_r( errorCode, errorMessageBuffer, MAX_LENGTH );
#endif

#  undef STRERROR_R_RETURNS_INT

#endif /* !ODDSOURCE_IS_WINDOWS */

        return errorMessage.empty() ? "Unknown error"s : errorMessage;
    }
}
