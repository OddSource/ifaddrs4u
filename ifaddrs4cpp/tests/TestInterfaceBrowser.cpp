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

#include <oddsource/network/interfaces/Interfaces.hpp>
#include "main.h"

using namespace OddSource::Interfaces;

class TestInterfaceBrowser : public Tests::Test
{
public:
    TestInterfaceBrowser()
    {
        add_test( test_print_all );
        add_test( test_get_by_name );
        add_test( test_get_by_index );
        add_test( test_get_interfaces );
    }

    void
    test_print_all()
    {
        using namespace ::std::string_literals;

        uint32_t interfacesFound{ 0 };
        bool loopbackFound{ false };
        bool nonLoopbackFound{ false };

        ::std::cout << "Interface browser output below: " << ::std::endl;
        assert_that( this->_browser.for_each_interface(
            [ this, &interfacesFound, &loopbackFound, &nonLoopbackFound ]
            ( Interface const & rInterface )
            {
                ++interfacesFound;
                if ( rInterface.is_loopback() )
                {
                    loopbackFound = true;
                }
                else
                {
                    nonLoopbackFound = true;
                }

                ::std::cout << rInterface << ::std::endl;
                assert_that( rInterface.index() > 0 );
                assert_not_that( rInterface.name().empty() );
                assert_not_that( rInterface.friendlyName().empty() );
                assert_not_that( rInterface.description().empty() );
                return true;
            } ) );

        assert_that(
            interfacesFound > 1,
            "At least two interfaces should have been found, but "s + ::std::to_string( interfacesFound ) +
                " found instead."s );
        assert_that( loopbackFound, "At least one loopback interface should have been found." );
        assert_that( nonLoopbackFound, "At least one non-loopback interface should have been found." );
    }

    void
    test_get_by_name()
    {
#ifdef ODDSOURCE_IS_WINDOWS

        // TODO dunno yet

#elif ODDSOURCE_IS_MACOS /* ODDSOURCE_IS_WINDOWS */

        auto const pLo( this->_browser.get_interface( "lo0" ) );
        assert_equals( pLo->name(), "lo0" );
        assert_not_that( pLo->friendlyName().empty() );
        assert_not_that( pLo->description().empty() );
        assert_that( pLo->index() > 0 );

        auto const lo( this->_browser[ "lo0" ] );
        assert_equals( lo.name(), "lo0" );
        assert_that( lo.index() > 0 );

        assert_equals( pLo->friendlyName(), lo.friendlyName() );
        assert_equals( pLo->description(), lo.description() );
        assert_equals( pLo->index(), lo.index() );

        auto const pEn( this->_browser.get_interface( "en0" ) );
        assert_equals( pEn->name(), "en0" );
        assert_not_that( pEn->friendlyName().empty() );
        assert_not_that( pEn->description().empty() );
        assert_that( pEn->index() > 0 );

        auto const en( this->_browser[ "en0" ] );
        assert_equals( en.name(), "en0" );
        assert_that( en.index() > 0 );

        assert_equals( pEn->friendlyName(), en.friendlyName() );
        assert_equals( pEn->description(), en.description() );
        assert_equals( pEn->index(), en.index() );

#else /* !ODDSOURCE_IS_WINDOWS && !ODDSOURCE_IS_MACOS */

        // TODO dunno yet

#endif /* !ODDSOURCE_IS_WINDOWS && !ODDSOURCE_IS_MACOS */

        assert_throws( this->_browser[ "fooBar42" ], ::std::out_of_range );
        assert_equals( this->_browser.get_interface( "fooBar42" ), nullptr );
    }

    void
    test_get_by_index()
    {
#ifdef ODDSOURCE_IS_WINDOWS

        // TODO dunno yet

#elif ODDSOURCE_IS_MACOS /* ODDSOURCE_IS_WINDOWS */

        auto const pIf0( this->_browser.get_interface( 1 ) );
        assert_not_that( pIf0->name().empty() );
        assert_not_that( pIf0->friendlyName().empty() );
        assert_not_that( pIf0->description().empty() );
        assert_equals( pIf0->index(), 1u );

        auto const if0( this->_browser[ 1 ] );
        assert_equals( if0.index(), 1u );

        assert_equals( pIf0->name(), if0.name() );
        assert_equals( pIf0->friendlyName(), if0.friendlyName() );
        assert_equals( pIf0->description(), if0.description() );

        auto const pIf1( this->_browser.get_interface( 2 ) );
        assert_not_that( pIf1->name().empty() );
        assert_not_that( pIf1->friendlyName().empty() );
        assert_not_that( pIf1->description().empty() );
        assert_equals( pIf1->index(), 2u );

        auto const if1( this->_browser[ 2 ] );
        assert_equals( if1.index(), 2u );

        assert_equals( pIf1->name(), if1.name() );
        assert_equals( pIf1->friendlyName(), if1.friendlyName() );
        assert_equals( pIf1->description(), if1.description() );

#else /* !ODDSOURCE_IS_WINDOWS && !ODDSOURCE_IS_MACOS */

        // TODO dunno yet

#endif /* !ODDSOURCE_IS_WINDOWS && !ODDSOURCE_IS_MACOS */

        assert_throws( this->_browser[ 4'294'967'294 ], ::std::out_of_range );
        assert_equals( this->_browser.get_interface( 4'294'967'294 ), nullptr );
    }

    void
    test_get_interfaces()
    {
        using namespace ::std::string_literals;

        uint32_t interfacesFound{ 0 };
        bool loopbackFound{ false };
        bool nonLoopbackFound{ false };
        for ( auto const & pInterface : this->_browser.get_interfaces() )
        {
            ++interfacesFound;
            if ( pInterface->is_loopback() )
            {
                loopbackFound = true;
            }
            else
            {
                nonLoopbackFound = true;
            }

            assert_that( pInterface->index() > 0 );
            assert_not_that( pInterface->name().empty() );
            assert_not_that( pInterface->friendlyName().empty() );
            assert_not_that( pInterface->description().empty() );
        }

        assert_that(
            interfacesFound > 1,
            "At least two interfaces should have been found, but "s + ::std::to_string( interfacesFound ) +
                " found instead."s );
        assert_that( loopbackFound, "At least one loopback interface should have been found." );
        assert_that( nonLoopbackFound, "At least one non-loopback interface should have been found." );
    }

    [[maybe_unused]]
    static
    std::unique_ptr< Test >
    create()
    {
        return std::make_unique< TestInterfaceBrowser >();
    }

private:
    InterfaceBrowser _browser;
};

namespace
{
    [[maybe_unused]]
    Tests::Test::Registrar< TestInterfaceBrowser > registrar( "TestInterfaceBrowser" );
}
