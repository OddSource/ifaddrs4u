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

#include "config.h"
#include "helpers.h"
#include "interface.h"
#include "ip_address.h"
#include "mac_address.h"
#include "macros.h"
#include "version.h"

#include "structmember.h"  // not included in Python.h

#include <sstream>
#include <stdexcept>

#include <oddsource/network/interfaces/Interfaces.hpp>

static PyObject * IllegalStateError;

typedef struct
{
    PyObject_HEAD
    ::std::unique_ptr< OddSource::Interfaces::InterfaceBrowser > browser;
    PyTupleObject * interfaces;  // tuple
} InterfaceBrowser_PyObj;

static
int
InterfaceBrowser___init__(
    InterfaceBrowser_PyObj * self,
    PyObject * args,
    PyObject * kwargs )
{
    if ( PyTuple_Size( args ) > 0 || ( kwargs != NULL && PyDict_Size( kwargs ) > 0 ) )
    {
        PyErr_SetString( PyExc_TypeError, "ifaddrs4py.extern.InterfaceBrowser() takes no arguments" );
        return -1;
    }

    try
    {
        self->browser = ::std::make_unique< OddSource::Interfaces::InterfaceBrowser >();
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET( PyExc_RuntimeError, return -1 )

    if ( self->interfaces != NULL )
    {
        PyTupleObject * tmp = self->interfaces;
        self->interfaces = NULL;
        Py_DECREF( tmp );
    }

    return 0;
}

static
void
InterfaceBrowser_dealloc(
    InterfaceBrowser_PyObj * self )
{
    self->browser.reset();
    Py_XDECREF( self->interfaces );
    Py_TYPE( self )->tp_free( reinterpret_cast< PyObject * >( self ) );
}

static
PyObject *
InterfaceBrowser___repr__(
    InterfaceBrowser_PyObj * self )
{
    ::std::ostringstream oss;
    oss << "<ifaddrs4py.extern.InterfaceBrowser: ";
    if ( !self->browser )
    {
        oss << "uninitialized";
    }
    else if( self->interfaces == NULL )
    {
        oss << "pending population";
    }
    else
    {
        oss << "contains info on "
            << ::std::to_string( PyTuple_Size( reinterpret_cast< PyObject * >( self->interfaces ) ) )
            << " interfaces";
    }
    oss << ">";

    ::std::string const repr( oss.str() );
    return PyUnicode_FromStringAndSize( repr.c_str(), repr.length() );
}

static
PyTupleObject *
_get_interfaces(
    OddSource::Interfaces::InterfaceBrowser & browser )
{
    try
    {
        auto const interfaces( browser.get_interfaces() );

        PyObject * tuple = PyTuple_New( interfaces.size() );
        if( tuple == NULL )
        {
            return NULL;
        }

        int i{ 0 };
        for ( auto const & pInterface : interfaces )
        {
            try
            {
                PyObject * item( OddSource::ifaddrs4py::convertToPython( *pInterface ) );
                if ( PyTuple_SetItem( tuple, i++, item ) != 0 )
                {
                    Py_XDECREF( item );
                    return NULL;
                }
            }
            CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET( PyExc_RuntimeError, return NULL )
        }

        return reinterpret_cast< PyTupleObject * >( tuple );
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET( PyExc_RuntimeError, return NULL )
}

#define ENSURE_INITIALIZED(ret) if ( !self->browser ) \
    { \
        PyErr_SetString( IllegalStateError, "ifaddrs4py.extern.InterfaceBrowser() has not been initialized" ); \
        return ret; \
    }

namespace
{
    PyObject *
    forEachPopulatedInterface(
        InterfaceBrowser_PyObj * self,
        PyObject * callable )
    {
        Py_ssize_t numInterfaces{ PyTuple_Size( reinterpret_cast< PyObject * >( self->interfaces ) ) };
        if ( PyErr_Occurred() != NULL )
        {
            return NULL;
        }

        for ( Py_ssize_t i{ 0 }; i < numInterfaces; ++i )
        {
            PyObject * interfacePy( PyTuple_GetItem( reinterpret_cast< PyObject * >( self->interfaces ), i ) );
            if (interfacePy == NULL)
            {
                return NULL;
            }

            Py_INCREF(interfacePy);
            PyObject * result( PyObject_CallFunctionObjArgs( callable, interfacePy, NULL ) );
            Py_DECREF(interfacePy);
            if ( result == NULL )
            {
                return NULL;
            }
            if ( !PyBool_Check( result ) )
            {
                ::std::ostringstream oss;
                oss << "InterfaceBrowser.for_each_interface() expected second argument to be a callable that returns "
                    << "a bool, but instead it returned a " << result->ob_type->tp_name << ".";
                PyErr_SetString( PyExc_ValueError, oss.str().c_str() );
                Py_DECREF( result );
                return NULL;
            }
            if ( !PyObject_IsTrue( result ) )
            {
                return result;
            }
        }

        return Py_True;
    }

    PyObject *
    forEachInterfaceAndPopulate(
        InterfaceBrowser_PyObj * self,
        PyObject * callable )
    {
        bool keepCallingCallable{ true };
        bool returnError{ false };

        PyObject * tempList( PyList_New( 0 ) );
        if ( tempList == NULL )
        {
            return NULL;
        }

        ::std::function< bool( Interface const & )> doThis =
        [ callable, &keepCallingCallable, &returnError, &tempList ]
        ( Interface const & rInterface )
        {
            PyObject * interfacePy;
            try
            {
                interfacePy = OddSource::ifaddrs4py::convertToPython( rInterface );
            }
            CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET( PyExc_RuntimeError, returnError = true; return false )

            if ( !returnError && keepCallingCallable )
            {
                Py_INCREF( interfacePy );
                PyObject * result = PyObject_CallFunctionObjArgs( callable, interfacePy, NULL );
                Py_DECREF( interfacePy );
                if ( result == NULL )
                {
                    returnError = true;
                }
                else if ( !PyBool_Check( result ) )
                {
                    ::std::ostringstream oss;
                    oss << "InterfaceBrowser.for_each_interface() expected second argument to be a callable that "
                        << "returns a bool, but instead it returned a " << result->ob_type->tp_name << ".";
                    PyErr_SetString( PyExc_ValueError, oss.str().c_str() );
                    returnError = true;
                }
                else
                {
                    keepCallingCallable = PyObject_IsTrue( result ) ? true : false;
                }
                Py_DECREF( result );
            }

            if ( !returnError && PyList_Append( tempList, interfacePy ) != 0 )
            {
                returnError = true;
            }

            Py_DECREF( interfacePy );
            return !returnError;
        };

        self->browser->for_each_interface( doThis );

        if ( !returnError && self->interfaces == NULL )
        {
            self->interfaces = reinterpret_cast< PyTupleObject * >( PyList_AsTuple( tempList ) );
            if ( self->interfaces == NULL )
            {
                returnError = true;
            }
        }

        Py_DECREF( tempList );

        if ( returnError )
        {
            return NULL;
        }

        return keepCallingCallable ? Py_True : Py_False;
    }
}

static
PyObject *
InterfaceBrowser_for_each_interface(
    InterfaceBrowser_PyObj * self,
    PyObject * const * args,
    Py_ssize_t nargs )
{
    if ( nargs != 1 )
    {
        ::std::ostringstream oss;
        oss << "InterfaceBrowser.for_each_interface() expected 2 arguments, but "
            << ::std::to_string( nargs + 1 ) << " given.";
        PyErr_SetString( PyExc_ValueError, oss.str().c_str() );
        return NULL;
    }

    ENSURE_INITIALIZED( NULL )

    PyObject * callable( args[ 0 ] );
    if ( !PyCallable_Check( callable ) )
    {
        ::std::ostringstream oss;
        oss << "InterfaceBrowser.for_each_interface() expected second argument to be callable, not "
            << callable->ob_type->tp_name << ".";
        PyErr_SetString( PyExc_ValueError, oss.str().c_str() );
        return NULL;
    }

    if ( self->interfaces == NULL )
    {
        return forEachInterfaceAndPopulate( self, callable );
    }
    return forEachPopulatedInterface( self, callable );
}

#define ENSURE_POPULATED(ret) if ( self->interfaces == NULL ) \
    { \
        self->interfaces = _get_interfaces( *self->browser ); \
        if ( self->interfaces == NULL ) \
        { \
            return ret; \
        } \
    }

static
PyObject *
InterfaceBrowser_get_interfaces(
    InterfaceBrowser_PyObj * self,
    PyObject * Py_UNUSED( args ) )
{
    ENSURE_INITIALIZED( NULL )
    ENSURE_POPULATED( NULL )
    Py_INCREF( self->interfaces );
    return reinterpret_cast< PyObject * >( self->interfaces );
}

static
PyObject *
InterfaceBrowser___iter__(
    InterfaceBrowser_PyObj * self,
    PyObject * args )
{
    return PyObject_GetIter( InterfaceBrowser_get_interfaces( self, args ) );
}

static
Py_ssize_t
InterfaceBrowser___len__(
    InterfaceBrowser_PyObj * self )
{
    ENSURE_INITIALIZED( -1 )

    if ( self->interfaces != NULL )
    {
        return PyTuple_Size( reinterpret_cast< PyObject * >( self->interfaces ) );
    }

    try
    {
        return self->browser->get_interfaces().size();
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET( PyExc_RuntimeError, return -1 )
}

static
PyObject *
InterfaceBrowser_length(
    InterfaceBrowser_PyObj * self,
    PyObject * Py_UNUSED( args ) )
{
    Py_ssize_t const length{ InterfaceBrowser___len__( self ) };
    if ( length == -1 )
    {
        return NULL;
    }
    return PyLong_FromSsize_t( length );
}

static
PyObject *
InterfaceBrowser___getitem___mapping(
    InterfaceBrowser_PyObj * self,
    PyObject * key )
{
    ENSURE_INITIALIZED( NULL )
    if ( PyUnicode_Check( key ) )
    {
        Py_ssize_t size{ 0 };
        char const * chars( PyUnicode_AsUTF8AndSize( key, &size ) );
        if ( chars && size > 0 )
        {
            try
            {
                return OddSource::ifaddrs4py::convertToPython( ( *self->browser )[ chars ] );
            }
            CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET( PyExc_KeyError, return NULL )
        }
        else
        {
            if ( PyErr_Occurred() == NULL )
            {
                PyErr_SetString( PyExc_ValueError, "Interface key must be a str or int" );
            }
            return NULL;
        }
    }
    else if( PyLong_Check( key ) )
    {
        try
        {
            return OddSource::ifaddrs4py::convertToPython( (*self->browser)[ PyLong_AsUnsignedLong( key ) ] );
        }
        CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET( PyExc_KeyError, return NULL )
    }
    else
    {
        if ( PyErr_Occurred() == NULL )
        {
            PyErr_SetString( PyExc_ValueError, "Interface key must be a str or int" );
        }
        return NULL;
    }
}

static
PyObject *
InterfaceBrowser_get_interface(
    InterfaceBrowser_PyObj * self,
    PyObject * const * args,
    Py_ssize_t nargs )
{
    if ( nargs != 1 )
    {
        ::std::ostringstream oss;
        oss << "InterfaceBrowser.get_interface() expected 2 arguments, but "
            << ::std::to_string( nargs + 1 ) << " given.";
        PyErr_SetString( PyExc_ValueError, oss.str().c_str() );
        return NULL;
    }

    ENSURE_INITIALIZED( NULL )
    ENSURE_POPULATED( NULL )
    return InterfaceBrowser___getitem___mapping( self, args[ 0 ] );
}

#ifndef ODDSOURCE_IS_WINDOWS
// "cast between incompatible function types from <...> to ‘PyCFunction’"
#  ifdef __clang__
#    pragma clang diagnostic push
#    pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#  else /* __clang__ */
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wcast-function-type"
#  endif /* !__clang__ */
#endif /* ODDSOURCE_IS_WINDOWS */

static struct PyMemberDef InterfaceBrowser_members [] =
{
    { NULL, 0, 0, 0, NULL }  /* Sentinel */
};

static struct PyMethodDef InterfaceBrowser_methods [] =
{
    { "get_interfaces", (PyCFunction) InterfaceBrowser_get_interfaces, METH_NOARGS, NULL },
    { "length", (PyCFunction) InterfaceBrowser_length, METH_NOARGS, NULL },
    { "get_interface", (PyCFunction) InterfaceBrowser_get_interface, METH_FASTCALL, NULL },
    { "for_each_interface", (PyCFunction) InterfaceBrowser_for_each_interface, METH_FASTCALL, NULL },
    { NULL, NULL, 0, NULL }  /* Sentinel */
};

static struct PyGetSetDef InterfaceBrowser_getters_and_setters [] =
{
    { NULL, NULL, NULL, NULL, NULL }  /* Sentinel */
};

static PyMappingMethods InterfaceBrowser_mapping =
{
    (lenfunc) InterfaceBrowser___len__, // .mp_length
    (binaryfunc) InterfaceBrowser___getitem___mapping, // .mp_subscript
    NULL, // .mp_ass_subscript
};

static PyTypeObject InterfaceBrowser_PyType =
{
    PyVarObject_HEAD_INIT( &PyType_Type, 0 )
    "ifaddrs4py.extern.InterfaceBrowser", // .tp_name
    sizeof( InterfaceBrowser_PyObj ), // .tp_basicsize
    0, // .tp_itemsize
    (destructor) InterfaceBrowser_dealloc, // .tp_dealloc
    0, // .tp_vectorcall_offset
    NULL, // .tp_getattr
    NULL, // .tp_setattr
    NULL, // .tp_as_async
    (reprfunc) InterfaceBrowser___repr__, // .tp_repr
    NULL, // .tp_as_number
    NULL, // .tp_as_sequence
    &InterfaceBrowser_mapping, // .tp_as_mapping
    NULL, // .tp_hash
    NULL, // .tp_call
    (reprfunc) InterfaceBrowser___repr__, // .tp_str
    NULL, // .tp_getattro
    NULL, // .tp_setattro
    NULL, // .tp_as_buffer
    Py_TPFLAGS_METHOD_DESCRIPTOR | Py_TPFLAGS_IMMUTABLETYPE, // .tp_flags
    NULL, // .tp_doc
    NULL, // .tp_traverse
    NULL, // .tp_clear
    NULL, // .tp_richcompare
    0, // .tp_weaklistoffset
    (getiterfunc) InterfaceBrowser___iter__, // .tp_iter
    NULL, // .tp_iternext
    InterfaceBrowser_methods, // .tp_methods
    InterfaceBrowser_members, // .tp_members
    InterfaceBrowser_getters_and_setters, // .tp_getset
    NULL, // .tp_base
    NULL, // .tp_dict
    NULL, // .tp_descr_get
    NULL, // .tp_descr_set
    0, // .tp_dictoffset
    (initproc) InterfaceBrowser___init__, // .tp_init
    NULL, // .tp_alloc
    PyType_GenericNew, // .tp_new
    NULL, // .tp_free
    NULL, // .tp_is_gc
    NULL, // .tp_bases
    NULL, // .tp_mro
    NULL, // .tp_cache
    NULL, // .tp_subclasses
    NULL, // .tp_weaklist
    NULL, // .tp_del
    0, // .tp_version_tag
    NULL, // .tp_finalize
    NULL, // .tp_vectorcall
#if PY_MINOR_VERSION > 11
    0, // .tp_watched added in 3.12
#endif /* PY_MINOR_VERSION > 11 */
#if PY_MINOR_VERSION > 12
    0, // .tp_versions_used added in 3.13
#endif /* PY_MINOR_VERSION > 12 */
};

static
int
ifaddrs4py_module_init(
    PyObject * moduleSelf )
{
    using namespace OddSource::ifaddrs4py;

    void * void_state_wrapper( PyModule_GetState( moduleSelf ) );
    IF_NULL_RETURN_INT( void_state_wrapper )
    auto state_wrapper( reinterpret_cast< ModuleStateWrapper * >( void_state_wrapper ) );

    auto pState( ::std::make_shared< ModuleState >() );
    state_wrapper->state = ::std::move( pState );

    try
    {
        OddSource::ifaddrs4py::initVersionConstants( moduleSelf );
        OddSource::ifaddrs4py::initInterfaceEnums( moduleSelf );
        OddSource::ifaddrs4py::initIPAddressSamples( moduleSelf );
        OddSource::ifaddrs4py::initMacAddressSamples( moduleSelf );
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET( PyExc_ImportError, { return -1; } )

    IllegalStateError = PyErr_NewException( "ifaddrs4py.extern.IllegalStateError", PyExc_RuntimeError, NULL );
    if ( IllegalStateError == NULL )
    {
        return -1;
    }

    Py_INCREF( IllegalStateError );
    if ( PyModule_AddObject( moduleSelf, "IllegalStateError", IllegalStateError ) != 0 )
    {
        Py_DECREF( IllegalStateError );
        return -1;
    }

    Py_INCREF( &InterfaceBrowser_PyType );
    if ( PyModule_AddObject(
        moduleSelf,
        "InterfaceBrowser",
        reinterpret_cast< PyObject * >( &InterfaceBrowser_PyType ) ) != 0 )
    {
        Py_DECREF( IllegalStateError );
        Py_DECREF( &InterfaceBrowser_PyType );
        return -1;
    }

    return 0;
}

static
int
ifaddrs4py_module_traverse(
    PyObject * moduleSelf,
    visitproc visit,
    void * arg  )
{
    using namespace OddSource::ifaddrs4py;

    return ModuleState::getStateOf( moduleSelf )->gcVisit( visit, arg );
}

static
void
ifaddrs4py_module_free(
    PyObject * moduleSelf )
{
    using namespace OddSource::ifaddrs4py;

    ModuleState::getStateOf( moduleSelf ).reset();
}

static struct PyMethodDef ifaddrs4py_module_methods [] =
{
    { "get_sample_interface_ipv4_address", extern_get_sample_interface_ipv4_address, METH_NOARGS, NULL },
    { "get_sample_interface_ipv6_address", extern_get_sample_interface_ipv6_address, METH_NOARGS, NULL },
    { "get_sample_interface_scoped_ipv6_address", extern_get_sample_interface_scoped_ipv6_address, METH_NOARGS, NULL },
    { "get_sample_interface", extern_get_sample_interface, METH_NOARGS, NULL },
    { "get_mac_address_data_from_repr", (PyCFunction) extern_get_mac_address_data_from_repr, METH_FASTCALL, NULL },
    { "get_mac_address_repr_from_data", (PyCFunction) extern_get_mac_address_repr_from_data, METH_FASTCALL, NULL },
    { NULL, NULL, 0, NULL }        /* Sentinel */
};

static PyModuleDef_Slot ifaddrs4py_module_slots [] =
{
    { Py_mod_exec, reinterpret_cast< void * >( ifaddrs4py_module_init ) },
    { 0, NULL }
};

static struct PyModuleDef ifaddrs4py_module =
{
    PyModuleDef_HEAD_INIT, // .m_base
    "ifaddrs4py.extern", // .m_name, name of module
    PyDoc_STR(
        "The native interface between ifaddrs4py and ifaddrs4cpp. You should not use this directly, but "
        "rather should use the pure Python modules wrapping it." ), // .m_doc, module documentation
    sizeof( OddSource::ifaddrs4py::ModuleStateWrapper ), // .m_size, size of per-interpreter state of the module
    ifaddrs4py_module_methods, // .m_methods
    ifaddrs4py_module_slots, // .m_slots
    ifaddrs4py_module_traverse, // .m_traverse
    NULL, // .m_clear
    (freefunc) ifaddrs4py_module_free, // .m_free
};

#ifndef ODDSOURCE_IS_WINDOWS
#  ifdef __clang__
#    pragma clang diagnostic pop
#  else
#    pragma GCC diagnostic pop
#  endif /* __clang__ */
#endif /* ODDSOURCE_IS_WINDOWS */

PyMODINIT_FUNC
PyInit_extern(
    void )
{
    if ( PyType_Ready( &InterfaceBrowser_PyType ) != 0 )
    {
        return NULL;
    }
    return PyModuleDef_Init( &ifaddrs4py_module );
}
