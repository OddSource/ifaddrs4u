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

#include "mac_address.h"

#include <sstream>

namespace
{
    inline
    PyObject *
    dataToTuple(
        OddSource::Interfaces::MacAddress const & address )
    {
        auto const data( static_cast< uint8_t const * >( address ) );
        auto const dataLength( address.length() );

        PyObject * tuple( PyTuple_New( dataLength ) );
        if( tuple == NULL )
        {
            throw ::std::runtime_error( "Unable to create tuple" );
        }
        for( Py_ssize_t i{ 0 }; i < dataLength; ++i )
        {
            PyObject * item( PyLong_FromUnsignedLong( data[ i ] ) );
            if( item == NULL )
            {
                Py_DECREF( tuple );
                throw ::std::runtime_error( "Unable to create long" );
            }

            if ( PyTuple_SetItem( tuple, i, item ) != 0 )
            {
                Py_DECREF( tuple );
                Py_XDECREF( item );
                throw ::std::runtime_error( "Unable to place long in tuple" );
            }
        }

        return tuple;
    }
}

PyObject *
OddSource::ifaddrs4py::
convertToPython(
    OddSource::Interfaces::MacAddress const & address )
{
    PyObject * MacAddress( getModuleClass( "ifaddrs4py.mac_address", "MacAddress" ) );

    ::std::string repr( address );
    PyObject * tuple;
    try
    {
        tuple = dataToTuple( address );
    }
    catch ( ::std::exception const & )
    {
        Py_DECREF( MacAddress );
        throw;
    }

    PyObject * args( Py_BuildValue( "(s#O)", repr.c_str(), repr.length(), tuple ) );
    Py_DECREF( tuple );
    if ( args == NULL )
    {
        Py_DECREF( MacAddress );
        throw ::std::runtime_error( "Unable to create args list" );
    }

    PyObject * instance( PyObject_Call( MacAddress, args, NULL ) );
    Py_DECREF( MacAddress );
    Py_DECREF( args );
    if ( instance == NULL )
    {
        throw ::std::runtime_error( "Unable to instantiate ifaddrs4py.mac_address.MacAddress class" );
    }

    return instance;
}

PyObject *
extern_get_mac_address_data_from_repr(
    PyObject * Py_UNUSED( moduleSelf ),
    PyObject * const * args,
    Py_ssize_t nargs )
{
    if ( nargs != 1 )
    {
        ::std::ostringstream oss;
        oss << "ifaddrs4py.extern.get_mac_address_data_from_repr expected 1 argument, but "
            << ::std::to_string( nargs ) << " given.";
        PyErr_SetString( PyExc_ValueError, oss.str().c_str() );
        return NULL;
    }

    Py_ssize_t size{ 0 };
    char const * chars( PyUnicode_AsUTF8AndSize( args[ 0 ], &size ) );
    if ( !chars || size < 1 )
    {
        ::std::string reprCpp;
        PyObject * reprPy( PyObject_Str( args[ 0 ] ) );
        if ( reprPy )
        {
            char const * reprC( PyUnicode_AsUTF8AndSize( reprPy, &size ) );
            if ( reprC && size > 0 )
            {
                reprCpp = reprC;
            }
            else
            {
                reprCpp = "[unknown]";
            }
            Py_DECREF( reprPy );
        }
        ::std::ostringstream oss;
        oss << "ifaddrs4py.extern.get_mac_address_data_from_repr expected string argument, but "
            << reprCpp << " given.";
        PyErr_SetString( PyExc_ValueError, oss.str().c_str() );
        return NULL;
    }

    try
    {
        return dataToTuple( OddSource::Interfaces::MacAddress( chars ) );
    }
    catch ( ::std::exception const & e )
    {
        PyErr_SetString( PyExc_ValueError, e.what() );
        return NULL;
    }
}

PyObject *
extern_get_mac_address_repr_from_data(
    PyObject * Py_UNUSED(moduleSelf),
    PyObject * const * args,
    Py_ssize_t nargs )
{
    if ( nargs != 1 )
    {
        ::std::ostringstream oss;
        oss << "ifaddrs4py.extern.get_mac_address_repr_from_data expected 1 argument, but "
            << ::std::to_string( nargs ) << " given.";
        PyErr_SetString( PyExc_ValueError, oss.str().c_str() );
        return NULL;
    }

    if ( !PyTuple_Check( args[ 0 ] ) )
    {
        ::std::string reprCpp;
        Py_ssize_t size{ 0 };
        PyObject * reprPy( PyObject_Str( args[ 0 ] ) );
        if ( reprPy )
        {
            char const * reprC( PyUnicode_AsUTF8AndSize( reprPy, &size ) );
            if ( reprC && size > 0 )
            {
                reprCpp = reprC;
            }
            else
            {
                reprCpp = "[unknown]";
            }
            Py_DECREF( reprPy );
        }

        ::std::ostringstream oss;
        oss << "ifaddrs4py.extern.get_mac_address_repr_from_data expected tuple argument, but "
            << reprCpp << " given.";
        PyErr_SetString( PyExc_ValueError, oss.str().c_str() );
        return NULL;
    }

    Py_ssize_t dataLength{ PyTuple_GET_SIZE( args[ 0 ] ) };
    if ( PyErr_Occurred() != NULL )
    {
        return NULL;
    }

    auto data( ::std::make_unique< uint8_t[] >( dataLength ) );
    for( Py_ssize_t i{ 0 }; i < dataLength; i++ )
    {
        PyObject * item( PyTuple_GetItem( args[0], i ) );
        if ( PyErr_Occurred() != NULL )
        {
            return NULL;
        }
        data[ i ] = static_cast< uint8_t >( PyLong_AsUnsignedLong( item ) );
        if ( PyErr_Occurred() != NULL )
        {
            return NULL;
        }
    }

    try
    {
        ::std::string const repr( OddSource::Interfaces::MacAddress( data.get(), dataLength ) );
        return PyUnicode_FromStringAndSize( repr.c_str(), repr.length() );
    }
    catch ( ::std::exception const & e )
    {
        PyErr_SetString( PyExc_ValueError, e.what() );
        return NULL;
    }
}

void
OddSource::ifaddrs4py::
initMacAddressSamples(
    PyObject * module)
{
    static OddSource::Interfaces::MacAddress const MAC( "ac:de:48:00:11:22" );

    PyObject * mac( convertToPython(MAC) );
    int result( PyModule_AddObjectRef( module, "_TEST_MAC_ADDRESS", mac ) );
    Py_DECREF( mac );
    if ( result != 0 )
    {
        throw ::std::runtime_error( "Unable to initialize _TEST_MAC_ADDRESS." );
    }
}
