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

#include "ip_address.h"

PyObject *
OddSource::ifaddrs4py::
convertToPython(
    OddSource::Interfaces::IPv4Address const & address )
{
    PyObject * IPv4Address( getModuleClass( "ifaddrs4py.ip_address", "IPv4Address" ) );

    auto const addr( static_cast< in_addr const * >( address ) );
    auto const ints( reinterpret_cast< uint32_t const * >( addr ) );

    PyObject * args( Py_BuildValue( "(I)", ntohl( *ints ) ) );
    if ( args == NULL )
    {
        Py_DECREF( IPv4Address );
        throw ::std::runtime_error( "Unable to create args list" );
    }

    PyObject * instance = PyObject_Call( IPv4Address, args, NULL );
    Py_DECREF( IPv4Address );
    Py_DECREF( args );
    if ( instance == NULL )
    {
        throw ::std::runtime_error( "Unable to instantiate ifaddrs4py.ip_address.IPv4Address class" );
    }

    return instance;
}

PyObject *
OddSource::ifaddrs4py::
convertToPython(
    OddSource::Interfaces::IPv6Address const & address )
{
    PyObject * IPv6Address( getModuleClass( "ifaddrs4py.ip_address", "IPv6Address" ) );

    auto const addr( static_cast< in6_addr const * >( address ) );
    auto const bytes( reinterpret_cast< uint8_t const * >( addr ) );

    PyObject * args( Py_BuildValue( "(y#)", bytes, 16 ) );
    if ( args == NULL )
    {
        Py_DECREF( IPv6Address );
        throw ::std::runtime_error( "Unable to create args list" );
    }

    PyObject * kwargs( NULL );
    if ( address.has_scope_id() )
    {
        PyObject * scopeIDPy( Py_None );
        if ( address.scope_id() )
        {
            scopeIDPy = PyLong_FromUnsignedLong( *address.scope_id() );
        }

        PyObject * scopeNamePy( Py_None );
        if ( address.scope_name() )
        {
            ::std::string const scopeName( *address.scope_name() );
            scopeNamePy = PyUnicode_FromStringAndSize( scopeName.c_str(), scopeName.length() );
        }

        kwargs = Py_BuildValue( "{s:O,s:O}", "scope_id", scopeNamePy, "scope_number", scopeIDPy );
        Py_XDECREF( scopeNamePy );
        Py_XDECREF( scopeIDPy );
        if ( kwargs == NULL )
        {
            Py_DECREF( IPv6Address );
            Py_DECREF( args );
            throw ::std::runtime_error( "Unable to create kwargs dict" );
        }
    }

    PyObject * instance = PyObject_Call( IPv6Address, args, kwargs );
    Py_DECREF( IPv6Address );
    Py_DECREF( args );
    Py_XDECREF( kwargs );
    if ( instance == NULL )
    {
        throw ::std::runtime_error( "Unable to instantiate ifaddrs4py.ip_address.IPv6Address class" );
    }

    return instance;
}

void
OddSource::ifaddrs4py::
initIPAddressSamples(
    PyObject * module )
{
    static OddSource::Interfaces::IPv4Address const LO_V4( "127.0.0.1" );
    static OddSource::Interfaces::IPv6Address const LO_V6( "::1" );

    PyObject * lo = convertToPython( LO_V4 );
    int result = PyModule_AddObjectRef( module, "_TEST_LOOPBACK_V4", lo );
    Py_DECREF( lo );
    if ( result != 0 )
    {
        throw ::std::runtime_error( "Unable to initialize _TEST_LOOPBACK_V4." );
    }

    lo = convertToPython( LO_V6 );
    result = PyModule_AddObjectRef( module, "_TEST_LOOPBACK_V6", lo );
    Py_DECREF( lo );
    if ( result != 0 )
    {
        throw ::std::runtime_error( "Unable to initialize _TEST_LOOPBACK_V6." );
    }
}