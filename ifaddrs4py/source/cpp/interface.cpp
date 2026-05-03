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

#include "interface.h"
#include "ip_address.h"
#include "mac_address.h"
#include "macros.h"

namespace
{
    template< typename F >
    PyObject *
    createEnumClass(
        PyObject * enumModule,
        char const * enumName,
        ::std::unordered_map< ::std::string, F const > const & values )
    {
        PyObject * enumClass = NULL;

        {
            PyObject * constantsDict = PyDict_New();
            for ( auto const & [ name, value ] : values )
            {
                PyDict_SetItemString(
                    constantsDict,
                    name.c_str(),
                    PyLong_FromLong( static_cast< ::std::underlying_type_t< F > >( value ) ) );
            }

            enumClass = PyObject_CallMethod(enumModule, "IntEnum", "sO", enumName, constantsDict);
            Py_CLEAR( constantsDict );
        }

        if ( enumClass == NULL )
        {
            throw ::std::runtime_error( "Failed to define new IntEnum class" );
        }

        return enumClass;
    }
}

template< class IPAddressT >
PyObject *
OddSource::ifaddrs4py::
convertToPython(
    OddSource::Interfaces::InterfaceIPAddress< IPAddressT > const & interfaceAddress )
{
    PyObject * address = NULL, * broadcastAddress = NULL, * pointToPointDestinationAddress = NULL,
             * flags = NULL, * prefixLength = NULL;
    PyObject * InterfaceIPAddress = NULL;

    try
    {
        address = convertToPython( interfaceAddress.address() );

        broadcastAddress = interfaceAddress.broadcast_address() ?
            convertToPython( *interfaceAddress.broadcast_address() ) :
            Py_None;

        pointToPointDestinationAddress = interfaceAddress.point_to_point_destination() ?
            convertToPython( *interfaceAddress.point_to_point_destination() ) :
            Py_None;

        flags = PyLong_FromUnsignedLong( interfaceAddress.flags() );
        if ( flags == NULL )
        {
            throw ::std::runtime_error( "Failed to create flags long" );
        }

        prefixLength = interfaceAddress.prefix_length() ?
            PyLong_FromUnsignedLong( *interfaceAddress.prefix_length() ) :
            Py_None;
        if ( prefixLength == NULL )
        {
            throw ::std::runtime_error( "Failed to create prefixLength long" );
        }

        InterfaceIPAddress = getModuleClass( "ifaddrs4py.interface", "InterfaceIPAddress" );
    }
    catch (...)
    {
        Py_XDECREF( address );
        Py_XDECREF( broadcastAddress );
        Py_XDECREF( pointToPointDestinationAddress );
        throw;
    }

    PyObject * args( PyTuple_New( 0 ) );
    PyObject * kwargs( Py_BuildValue(
        "{s:O,s:O,s:O,s:O,s:O}",
        "address", address,
        "flags", flags,
        "prefix_length", prefixLength,
        "broadcast_address", broadcastAddress,
        "point_to_point_destination", pointToPointDestinationAddress ) );
    Py_DECREF( address );
    Py_XDECREF( broadcastAddress );
    Py_XDECREF( pointToPointDestinationAddress );
    if ( args == NULL || kwargs == NULL )
    {
        Py_DECREF( InterfaceIPAddress );
        Py_XDECREF( args );
        Py_XDECREF( kwargs );
        throw ::std::runtime_error( "Unable to create args or kwargs" );
    }

    PyObject * instance = PyObject_Call( InterfaceIPAddress, args, kwargs );
    Py_DECREF( InterfaceIPAddress );
    Py_DECREF( args );
    Py_DECREF( kwargs );
    if ( instance == NULL )
    {
        throw ::std::runtime_error( "Unable to instantiate ifaddrs4py.interface.InterfaceIPAddress class" );
    }

    return instance;
}

PyObject *
OddSource::ifaddrs4py::
convertToPython(
    OddSource::Interfaces::Interface const & rInterface  )
{
    ::std::string const name( rInterface.name() );
    ::std::string const friendlyName( rInterface.friendlyName() );
    ::std::string const description( rInterface.description() );

    PyObject * index = NULL, * flags = NULL, * mtu = NULL, * macAddress = NULL,
             * ipv4Addresses = NULL, * ipv6Addresses = NULL;
    PyObject * Interface = NULL;

    try
    {
        index = PyLong_FromUnsignedLong( rInterface.index() );
        if ( index == NULL )
        {
            throw ::std::runtime_error( "Failed to create index long" );
        }

        flags = PyLong_FromUnsignedLong( rInterface.flags() );
        if ( flags == NULL )
        {
            throw ::std::runtime_error( "Failed to create flags long" );
        }

        if ( rInterface.mtu() )
        {
            mtu = PyLong_FromUnsignedLongLong( *rInterface.mtu() );
            if ( mtu == NULL )
            {
                throw ::std::runtime_error( "Unable to create MTU long" );
            }
        }
        else
        {
            mtu = Py_None;
        }

        macAddress = rInterface.mac_address() ?
            convertToPython( *rInterface.mac_address() ) :
            Py_None;

        ipv4Addresses = PyTuple_New( rInterface.ipv4_addresses().size() );
        if ( ipv4Addresses == NULL )
        {
            throw ::std::runtime_error( "Unable to create tuple of IPv4 interface addresses" );
        }
        int i{ 0 };
        for ( auto const & ipv4Address : rInterface.ipv4_addresses() )
        {
            PyObject * item = convertToPython( ipv4Address );
            if ( PyTuple_SetItem( ipv4Addresses, i++, item ) != 0 )
            {
                Py_XDECREF( item );
                throw ::std::runtime_error( "Unable to place IPv4 address into tuple" );
            }
        }

        ipv6Addresses = PyTuple_New( rInterface.ipv6_addresses().size() );
        if ( ipv6Addresses == NULL )
        {
            throw ::std::runtime_error( "Unable to create tuple of IPv6 interface addresses" );
        }
        i = 0;
        for ( auto const & ipv6Address : rInterface.ipv6_addresses() )
        {
            PyObject * item = convertToPython( ipv6Address );
            if ( PyTuple_SetItem( ipv6Addresses, i++, item ) != 0 )
            {
                Py_XDECREF( item );
                throw ::std::runtime_error( "Unable to place IPv6 address into tuple" );
            }
        }

        Interface = getModuleClass( "ifaddrs4py.interface", "Interface" );
    }
    catch (...)
    {
        Py_XDECREF( index );
        Py_XDECREF( flags );
        Py_XDECREF( mtu );
        Py_XDECREF( macAddress );
        Py_XDECREF( ipv4Addresses );
        Py_XDECREF( ipv6Addresses );
        throw;
    }

    PyObject * args( PyTuple_New( 0 ) );
    PyObject * kwargs( Py_BuildValue(
        "{s:O,s:s#,s:s#,s:s#,s:O,s:O,s:O,s:O,s:O}",
        "index", index,
        "name", name.c_str(), name.length(),
        "friendly_name", friendlyName.c_str(), friendlyName.length(),
        "description", description.c_str(), description.length(),
        "flags", flags,
        "mtu", mtu,
        "mac_address", macAddress,
        "ipv4_addresses", ipv4Addresses,
        "ipv6_addresses", ipv6Addresses ) );
    Py_XDECREF( mtu );
    Py_XDECREF( macAddress );
    Py_XDECREF( ipv4Addresses );
    Py_XDECREF( ipv6Addresses );
    if ( args == NULL || kwargs == NULL )
    {
        Py_DECREF( Interface );
        Py_XDECREF( args );
        Py_XDECREF( kwargs );
        throw ::std::runtime_error( "Unable to create args or kwargs" );
    }

    PyObject * instance = PyObject_Call( Interface, args, kwargs );
    Py_DECREF( Interface );
    Py_DECREF( args );
    Py_DECREF( kwargs );
    if ( instance == NULL )
    {
        throw ::std::runtime_error( "Unable to instantiate ifaddrs4py.interface.Interface class" );
    }

    return instance;
}

PyObject *
extern_get_sample_interface_ipv4_address(
    PyObject * Py_UNUSED( moduleSelf ),
    PyObject * Py_UNUSED( args ) )
{
    static OddSource::Interfaces::InterfaceIPv4Address const IPv4(
        OddSource::Interfaces::IPv4Address( "192.168.0.42" ),
        0,
        24u,
        OddSource::Interfaces::Broadcast,
        OddSource::Interfaces::IPv4Address( "192.168.0.254" ) );

    try
    {
        return OddSource::ifaddrs4py::convertToPython( IPv4 );
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET( PyExc_RuntimeError, return NULL )
}

PyObject *
extern_get_sample_interface_ipv6_address(
    PyObject * Py_UNUSED( moduleSelf ),
    PyObject * Py_UNUSED( args ) )
{
    using namespace OddSource::Interfaces;
    static InterfaceIPv6Address const IPv6(
        IPv6Address( "2001:470:2ccb:a61b:e:acf8:6736:d81e" ),
        InterfaceIPAddressFlag::AutoConfigured | InterfaceIPAddressFlag::Secured,
        56u );

    try
    {
        return OddSource::ifaddrs4py::convertToPython( IPv6 );
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET( PyExc_RuntimeError, return NULL )
}

PyObject *
extern_get_sample_interface_scoped_ipv6_address(
    PyObject * Py_UNUSED( moduleSelf ),
    PyObject * Py_UNUSED( args ) )
{
    using namespace OddSource::Interfaces;
    static InterfaceIPv6Address const Scoped_IPv6(
        IPv6Address(
            static_cast< in6_addr const * >( IPv6Address( "fe80::aede:48ff:fe00:1122" ) ),
            v6Scope{ 6, "en5" } ),
        0 | InterfaceIPAddressFlag::Secured,
        64u );

    try
    {
        return OddSource::ifaddrs4py::convertToPython( Scoped_IPv6 );
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET( PyExc_RuntimeError, return NULL )
}

PyObject *
extern_get_sample_interface(
    PyObject * Py_UNUSED( moduleSelf ),
    PyObject * Py_UNUSED( args ) )
{
    try
    {
        return OddSource::ifaddrs4py::convertToPython( OddSource::Interfaces::Interface::getSampleInterface() );
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET( PyExc_RuntimeError, return NULL )
}

void
OddSource::ifaddrs4py::
initInterfaceEnums(
    PyObject * module )
{
    PyObject * enumModule = getModule("enum");

    PyObject * enumClass;
    int result;

    using namespace OddSource::Interfaces;

    enumClass = createEnumClass< InterfaceIPAddressFlag >(
        enumModule,
        "InterfaceIPAddressFlag",
        InterfaceIPAddressFlag_Values );
    result = PyModule_AddObjectRef( module, "InterfaceIPAddressFlag", enumClass );
    Py_XDECREF( enumClass );
    if ( result != 0 )
    {
        throw ::std::runtime_error( "Failed to add enum to module" );
    }

    enumClass = createEnumClass< InterfaceFlag >(
        enumModule,
        "InterfaceFlag",
        InterfaceFlag_Values );
    result = PyModule_AddObjectRef( module, "InterfaceFlag", enumClass );
    Py_XDECREF( enumClass );
    if ( result != 0 )
    {
        throw ::std::runtime_error( "Failed to add enum to module" );
    }
}
