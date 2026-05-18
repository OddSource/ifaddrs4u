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

#include <oddsource/network/interfaces/detail/config.h>
#include <oddsource/network/interfaces/WSAHelper.hpp>

#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#ifdef ODDSOURCE_IS_WINDOWS
#include <errhandlingapi.h>
#include <crtdbg.h>
#endif /* ODDSOURCE_IS_WINDOWS */

#ifdef IFADDRS4CPP_INCLUDE_BOOST
#  include ODDSOURCE_BOOST_HEADER(exception/exception.hpp)
#  include ODDSOURCE_BOOST_HEADER(exception/diagnostic_information.hpp)
#endif /* IFADDRS4CPP_INCLUDE_BOOST */

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace
{
    class PopupDisabler
    {
    public:
        PopupDisabler()
            : disabled(false)
        {
#ifdef ODDSOURCE_IS_WINDOWS
            /*
             * Windows applications compiled debug will open a GUI popup alert window
             * when assertion failures and other serious issues occur. This can cause
             * GitHub Actions and other headless runners to block indefinitely with no
             * indication of the underlying issue. So we need to disable that behavior.
             */
            _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
            _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDERR );
            _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
            _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDERR );
            _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
            _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDERR );

            // prevent popups when terminate or abort is called
            SetErrorMode(
                    SEM_FAILCRITICALERRORS |
                    SEM_NOALIGNMENTFAULTEXCEPT |
                    SEM_NOGPFAULTERRORBOX | // if you're experiencing a crash and need a crash report, comment this line
                    SEM_NOOPENFILEERRORBOX );
#endif /* ODDSOURCE_IS_WINDOWS */
            disabled = true;
        }

        bool disabled;
    };

    [[maybe_unused]] PopupDisabler disabler;

#ifdef ODDSOURCE_IS_WINDOWS
    [[maybe_unused]] auto pWSAHelper( ::std::make_unique< OddSource::Interfaces::WinSockStartupCleanupHelper >() );
#endif /* ODDSOURCE_IS_WINDOWS */
}

template< typename T >
::std::string
OddSource::Interfaces::Tests::
type_id_string(
    T const & )
{
    return demangle( typeid( T ).name() );
}

template< typename T >
OddSource::Interfaces::Tests::
Test::Registrar< T >::
Registrar(
    ::std::string const & name )
{
    assert( disabler.disabled );
#ifdef ODDSOURCE_IS_WINDOWS
    assert( pWSAHelper );
#endif /* ODDSOURCE_IS_WINDOWS */
    Test::registrate( name, &T::create );
}

template< typename V1, typename V2 >
void
OddSource::Interfaces::Tests::
Test::
assert_equal(
    V1 const & arg1,
    char const * expression1,
    V2 const & arg2,
    char const * expression2,
    ::std::optional< ::std::string const > message,
    char const * file,
    int const line )
{
    this->_assertion_count++;
    if ( arg1 != arg2 )
    {
        ::std::ostringstream oss;
        oss << "assert " << expression1 << " == " << expression2 << " has failed ("
            << arg1 << " [" << type_id_string( arg1 ) << "] != "
            << arg2 << " [" << type_id_string( arg2 ) << "])";
        if ( message )
        {
            oss << ": " << *message;
        }

        this->failure( oss.str(), file, line );
    }
}

template< typename V1, typename V2 >
void
OddSource::Interfaces::Tests::
Test::
assert_not_equal(
    V1 const & arg1,
    char const * expression1,
    V2 const & arg2,
    char const * expression2,
    ::std::optional< ::std::string const > message,
    char const * file,
    int const line )
{
    this->_assertion_count++;
    if ( arg1 == arg2 )
    {
        ::std::ostringstream oss;
        oss << "assert " << expression1 << " != " << expression2 << " has failed ("
            << arg1 << " [" << type_id_string( arg1 ) << "] == "
            << arg2 << " [" << type_id_string( arg2 ) << "])";
        if ( message )
        {
            oss << ": " << *message;
        }

        this->failure( oss.str(), file, line );
    }
}

inline
void
OddSource::Interfaces::Tests::
Test::
assert_true(
    bool test,
    char const * expression,
    ::std::optional< ::std::string const > message,
    char const * file,
    int const line )
{
    this->_assertion_count++;
    if ( !test )
    {
        ::std::ostringstream oss;
        oss << "assert " << expression << " has failed (evaluated to false)";
        if ( message )
        {
            oss << ": " << *message;
        }

        this->failure( oss.str(), file, line );
    }
}

inline
void
OddSource::Interfaces::Tests::
Test::
assert_true(
    ::std::function< bool() > const & test,
    char const * expression,
    ::std::optional< ::std::string const > message,
    char const * file,
    int const line )
{
    this->assert_true( test(), expression, ::std::move( message ), file, line);
}

inline
void
OddSource::Interfaces::Tests::
Test::
assert_false(
    bool test,
    char const * expression,
    ::std::optional< ::std::string const > message,
    char const * file,
    int const line )
{
    this->_assertion_count++;
    if ( test )
    {
        ::std::ostringstream oss;
        oss << "assert !" << expression << " has failed (evaluated to true)";
        if ( message )
        {
            oss << ": " << *message;
        }

        this->failure( oss.str(), file, line );
    }
}

inline
void
OddSource::Interfaces::Tests::
Test::
assert_false(
    ::std::function< bool() > const & test,
    char const * expression,
    ::std::optional< ::std::string const > message,
    char const * file,
    int const line
    )
{
    this->assert_false( test(), expression, ::std::move( message ), file, line );
}

template< class E >
void
OddSource::Interfaces::Tests::
Test::
assert_except(
    ::std::function< void() > const & predicate,
    char const * expression,
    char const * exceptionType,
    ::std::optional< ::std::string const > exceptionMessageContains,
    ::std::optional< ::std::string const > message,
    char const * file,
    int const line )
{
    this->_assertion_count++;
    try
    {
        predicate();

        ::std::ostringstream oss;
        oss << "assert " << expression << " throws " << exceptionType;
        if ( exceptionMessageContains )
        {
            oss << " with a message containing \"" << *exceptionMessageContains << "\"";
        }
        oss << " has failed (no exception thrown)";
        if ( message )
        {
            oss << ": " << *message;
        }

        this->failure( oss.str(), file, line );
    }
    catch( E const & e )
    {
        if ( exceptionMessageContains )
        {
            if ( ::std::string const what( e.what() ); what.find( *exceptionMessageContains ) == ::std::string::npos )
            {
                ::std::ostringstream oss;
                oss << "assert " << expression << " throws " << exceptionType << " with a message containing \""
                    << *exceptionMessageContains
                    << "\" has failed (expected exception was thrown, but exception message \"" << what
                    << "\" did not contain expected string)";
                if ( message )
                {
                    oss << ": " << *message;
                }

                this->failure( oss.str(), file, line );
            }
        }
    }
    catch ( ::std::exception const & e )
    {
        ::std::ostringstream oss;
        oss << "assert " << expression << " throws " << exceptionType;
        if ( exceptionMessageContains )
        {
            oss << " with a message containing \"" << *exceptionMessageContains << "\"";
        }
        oss << " has failed (actual exception type: " << demangle( typeid( e ).name() ) << ", exception message: \""
            << e.what() << "\")";
        if ( message )
        {
            oss << ": " << *message;
        }

        this->failure( oss.str(), file, line );
    }
#ifdef IFADDRS4CPP_INCLUDE_BOOST
    catch ( IFADDRS4CPP_BOOST_NAMESPACE_ROOT::exception const & e )
    {
        ::std::ostringstream oss;
        oss << "assert " << expression << " throws " << exceptionType;
        if ( exceptionMessageContains )
        {
            oss << " with a message containing \"" << *exceptionMessageContains << "\"";
        }
        oss << " has failed (actual exception type: " << demangle( typeid( e ).name() )
            << ", diagnostic information: \""
            << IFADDRS4CPP_BOOST_NAMESPACE_ROOT::diagnostic_information( e ) << "\")";
        if ( message )
        {
            oss << ": " << *message;
        }

        this->failure( oss.str(), file, line );
    }
#endif /* IFADDRS4CPP_INCLUDE_BOOST */
    catch ( ... )
    {
        ::std::ostringstream oss;
        oss << "assert " << expression << " throws " << exceptionType;
        if ( exceptionMessageContains )
        {
            oss << " with a message containing \"" << *exceptionMessageContains << "\"";
        }
        oss << " has failed (unknown exception was caught)";
        if ( message )
        {
            oss << ": " << *message;
        }

        this->failure( oss.str(), file, line );
    }
}

inline
void
OddSource::Interfaces::Tests::
Test::
failure(
    ::std::string && message,
    char const * file,
    int const line )
{
    using namespace std::string_literals;
    this->_failures.emplace_back( file + ":"s + ::std::to_string( line ) + " - "s + message );
}

inline
void
OddSource::Interfaces::Tests::
Test::
error(
    ::std::string && message )
{
    this->_errors.emplace_back( message );
}
