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

#include "config.h"
#include "interface.h"
#include "ip_address.h"
#include "mac_address.h"
#include "macros.h"
#include "version.h"

#include "structmember.h"  // not include in Python.h

#include <sstream>
#include <stdexcept>

#include <ifaddrs4cpp/Interfaces.h>

#if PY_MINOR_VERSION < 10
int PyModule_AddObjectRef(PyObject * module, char const * name, PyObject * value)
{
    int result = PyModule_AddObject(module, name, value);
    if (result == 0)
    {
        // PyModule_AddObject is different from PyModule_AddObjectRef in that it "steals a reference to value on
        // success (if it returns 0)", but the caller is expecting to unconditionally Py_DECREF the value, so
        // increment the reference count to emulate the behavior of PyModule_AddObjectRef.
        Py_XINCREF(value);
    }
    return result;
}
#endif /* PY_MINOR_VERSION < 10 */

static PyObject * IllegalStateError;

typedef struct
{
    PyObject_HEAD
    ::std::unique_ptr<OddSource::Interfaces::InterfaceBrowser> browser;
    PyTupleObject * interfaces;  // tuple
} InterfaceBrowser_PyObj;

static int
InterfaceBrowser___init__(InterfaceBrowser_PyObj * self, PyObject * args, PyObject * kwargs)
{
    if (PyTuple_Size(args) > 0 || (kwargs != NULL && PyDict_Size(kwargs) > 0))
    {
        PyErr_SetString(PyExc_TypeError, "ifaddrs4py.extern.InterfaceBrowser() takes no arguments");
        return -1;
    }

    try
    {
        self->browser = ::std::make_unique<OddSource::Interfaces::InterfaceBrowser>();
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_RuntimeError, return -1)

    if (self->interfaces != NULL)
    {
        PyTupleObject * tmp = self->interfaces;
        self->interfaces = NULL;
        Py_DECREF(tmp);
    }

    return 0;
}

static void
InterfaceBrowser_dealloc(InterfaceBrowser_PyObj * self)
{
    self->browser.reset();
    Py_XDECREF(self->interfaces);
    Py_TYPE(self)->tp_free((PyObject *)self);
}

static PyObject *
InterfaceBrowser___repr__(InterfaceBrowser_PyObj * self)
{
    ::std::ostringstream oss;
    oss << "<ifaddrs4py.extern.InterfaceBrowser: ";
    if (!self->browser)
    {
        oss << "uninitialized";
    }
    else if(self->interfaces == NULL)
    {
        oss << "pending population";
    }
    else
    {
        oss << "contains info on " << ::std::to_string(PyTuple_Size((PyObject *)self->interfaces))
            << " interfaces";
    }
    oss << ">";

    ::std::string repr(oss.str());
    return PyUnicode_FromStringAndSize(repr.c_str(), repr.length());
}

static PyTupleObject *
_get_interfaces(OddSource::Interfaces::InterfaceBrowser & browser)
{
    try
    {
        auto interfaces(browser.get_interfaces());

        PyObject * tuple = PyTuple_New(interfaces.size());
        if(tuple == NULL)
        {
            return NULL;
        }

        int i(0);
        for (auto const & iface : interfaces)
        {
            try
            {
                PyObject * item = OddSource::ifaddrs4py::convert_to_python(iface);
                if (PyTuple_SetItem(tuple, i++, item) != 0)
                {
                    Py_XDECREF(item);
                    return NULL;
                }
            }
            CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_RuntimeError, return NULL)
        }

        return (PyTupleObject *)tuple;
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_RuntimeError, return NULL)
}

#define ENSURE_INITIALIZED(ret) if (!self->browser) \
    { \
        PyErr_SetString(IllegalStateError, "ifaddrs4py.extern.InterfaceBrowser() has not been initialized"); \
        return ret; \
    }

namespace
{
    PyObject * for_each_populated_interface(InterfaceBrowser_PyObj * self, PyObject * callable)
    {
        Py_ssize_t num_interfaces(PyTuple_Size((PyObject *) self->interfaces));
        if (PyErr_Occurred() != NULL)
        {
            return NULL;
        }

        for (Py_ssize_t i(0); i < num_interfaces; i++)
        {
            PyObject * iface(PyTuple_GetItem((PyObject *) self->interfaces, i));
            if (iface == NULL)
            {
                return NULL;
            }

            Py_INCREF(iface);
            PyObject * result = PyObject_CallFunctionObjArgs(callable, iface, NULL);
            Py_DECREF(iface);
            if (result == NULL)
            {
                return NULL;
            }
            if (!PyBool_Check(result))
            {
                ::std::ostringstream oss;
                oss << "InterfaceBrowser.for_each_interface() expected second argument to be a callable that returns "
                    << "a bool, but instead it returned a " << result->ob_type->tp_name << ".";
                PyErr_SetString(PyExc_ValueError, oss.str().c_str());
                Py_DECREF(result);
                return NULL;
            }
            if (!PyObject_IsTrue(result))
            {
                return result;
            }
        }

        return Py_True;
    }

    PyObject * for_each_interface_and_populate(InterfaceBrowser_PyObj * self, PyObject * callable)
    {
        bool keep_calling_callable(true);
        bool return_error(false);

        PyObject * temp_list(PyList_New(0));
        if (temp_list == NULL)
        {
            return NULL;
        }

        ::std::function<bool(Interface const &)> do_this =
        [callable, &keep_calling_callable, &return_error, &temp_list](auto cpp_interface)
        {
            PyObject * iface;
            try
            {
                iface = OddSource::ifaddrs4py::convert_to_python(cpp_interface);
            }
            CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_RuntimeError, return_error = true; return false)

            if (!return_error && keep_calling_callable)
            {
                Py_INCREF(iface);
                PyObject * result = PyObject_CallFunctionObjArgs(callable, iface, NULL);
                Py_DECREF(iface);
                if (result == NULL)
                {
                    return_error = true;
                }
                else if (!PyBool_Check(result))
                {
                    ::std::ostringstream oss;
                    oss << "InterfaceBrowser.for_each_interface() expected second argument to be a callable that "
                        << "returns a bool, but instead it returned a " << result->ob_type->tp_name << ".";
                    PyErr_SetString(PyExc_ValueError, oss.str().c_str());
                    return_error = true;
                }
                else
                {
                    keep_calling_callable = PyObject_IsTrue(result) ? true : false;
                }
                Py_DECREF(result);
            }

            if (!return_error && PyList_Append(temp_list, iface) != 0)
            {
                return_error = true;
            }

            Py_DECREF(iface);
            return !return_error;
        };

        self->browser->for_each_interface(do_this);

        if (!return_error && self->interfaces == NULL)
        {
            self->interfaces = (PyTupleObject *) PyList_AsTuple(temp_list);
            if (self->interfaces == NULL)
            {
                return_error = true;
            }
        }

        Py_DECREF(temp_list);

        if (return_error)
        {
            return NULL;
        }

        return keep_calling_callable ? Py_True : Py_False;
    }
}

static PyObject *
InterfaceBrowser_for_each_interface(InterfaceBrowser_PyObj * self, PyObject * const * args, Py_ssize_t nargs)
{
    if (nargs != 1)
    {
        ::std::ostringstream oss;
        oss << "InterfaceBrowser.for_each_interface() expected 2 arguments, but "
            << ::std::to_string(nargs + 1) << " given.";
        PyErr_SetString(PyExc_ValueError, oss.str().c_str());
        return NULL;
    }

    ENSURE_INITIALIZED(NULL)

    PyObject * callable(args[0]);
    if (!PyCallable_Check(callable))
    {
        ::std::ostringstream oss;
        oss << "InterfaceBrowser.for_each_interface() expected second argument to be callable, not "
            << callable->ob_type->tp_name << ".";
        PyErr_SetString(PyExc_ValueError, oss.str().c_str());
        return NULL;
    }

    if (self->interfaces == NULL)
    {
        return for_each_interface_and_populate(self, callable);
    }
    return for_each_populated_interface(self, callable);
}

#define ENSURE_POPULATED(ret) if (self->interfaces == NULL) \
    { \
        self->interfaces = _get_interfaces(*(self->browser)); \
        if (self->interfaces == NULL) \
        { \
            return ret; \
        } \
    }

static PyObject *
InterfaceBrowser_get_interfaces(InterfaceBrowser_PyObj * self, PyObject * Py_UNUSED(ignore))
{
    ENSURE_INITIALIZED(NULL)
    ENSURE_POPULATED(NULL)
    Py_INCREF(self->interfaces);
    return (PyObject *)self->interfaces;
}

static PyObject *
InterfaceBrowser___iter__(InterfaceBrowser_PyObj * self, PyObject * args)
{
    return PyObject_GetIter(InterfaceBrowser_get_interfaces(self, args));
}

static Py_ssize_t
InterfaceBrowser___len__(InterfaceBrowser_PyObj * self)
{
    ENSURE_INITIALIZED(-1)

    if (self->interfaces != NULL)
    {
        return PyTuple_Size((PyObject *) self->interfaces);
    }

    try
    {
        return self->browser->get_interfaces().size();
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_RuntimeError, return -1)
}

static PyObject *
InterfaceBrowser_length(InterfaceBrowser_PyObj * self, PyObject * Py_UNUSED(ignore))
{
    Py_ssize_t length(InterfaceBrowser___len__(self));
    if (length == -1)
    {
        return NULL;
    }
    return PyLong_FromSsize_t(length);
}

static PyObject *
InterfaceBrowser___getitem___mapping(InterfaceBrowser_PyObj * self, PyObject * key)
{
    ENSURE_INITIALIZED(NULL)
    if (PyUnicode_Check(key))
    {
        Py_ssize_t size(0);
        char const * chars(PyUnicode_AsUTF8AndSize(key, &size));
        if (chars)
        {
            try
            {
                return OddSource::ifaddrs4py::convert_to_python(self->browser->get_interface(chars));
            }
            CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_KeyError, return NULL)
        }
        else
        {
            if (PyErr_Occurred() == NULL)
            {
                PyErr_SetString(PyExc_ValueError, "Interface key must be a str or int");
            }
            return NULL;
        }
    }
    else if(PyLong_Check(key))
    {
        try
        {
            return OddSource::ifaddrs4py::convert_to_python(self->browser->get_interface(PyLong_AsUnsignedLong(key)));
        }
        CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_KeyError, return NULL)
    }
    else
    {
        if (PyErr_Occurred() == NULL)
        {
            PyErr_SetString(PyExc_ValueError, "Interface key must be a str or int");
        }
        return NULL;
    }
}

static PyObject *
InterfaceBrowser_get_interface(InterfaceBrowser_PyObj * self, PyObject * const * args, Py_ssize_t nargs)
{
    if (nargs != 1)
    {
        ::std::ostringstream oss;
        oss << "InterfaceBrowser.get_interface() expected 2 arguments, but "
            << ::std::to_string(nargs + 1) << " given.";
        PyErr_SetString(PyExc_ValueError, oss.str().c_str());
        return NULL;
    }

    ENSURE_INITIALIZED(NULL)
    ENSURE_POPULATED(NULL)
    return InterfaceBrowser___getitem___mapping(self, args[0]);
}

#ifndef IS_WINDOWS
#ifndef __clang__
#pragma GCC diagnostic push
// "cast between incompatible function types from <...> to ‘PyCFunction’"
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif /* __clang__ */
#endif /* IS_WINDOWS */

static struct PyMemberDef InterfaceBrowser_members [] =
{
    {NULL, 0, 0, 0, NULL}  /* Sentinel */
};

static struct PyMethodDef InterfaceBrowser_methods [] =
{
    {"get_interfaces", (PyCFunction) InterfaceBrowser_get_interfaces, METH_NOARGS, NULL},
    {"length", (PyCFunction) InterfaceBrowser_length, METH_NOARGS, NULL},
    {"get_interface", (PyCFunction) InterfaceBrowser_get_interface, METH_FASTCALL, NULL},
    {"for_each_interface", (PyCFunction) InterfaceBrowser_for_each_interface, METH_FASTCALL, NULL},
    {NULL, NULL, 0, NULL}  /* Sentinel */
};

static struct PyGetSetDef InterfaceBrowser_getters_and_setters [] =
{
    {NULL, NULL, NULL, NULL, NULL}  /* Sentinel */
};

static PyMappingMethods InterfaceBrowser_mapping =
{
    (lenfunc) InterfaceBrowser___len__, // .mp_length
    (binaryfunc) InterfaceBrowser___getitem___mapping, // .mp_subscript
    NULL, // .mp_ass_subscript
};

#ifndef IS_WINDOWS
#pragma GCC diagnostic push
// "error: 'tp_print' is deprecated" on Py3.8, but if you remove it, you get compiler errors about missing tp_print
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif /* IS_WINDOWS */

static PyTypeObject InterfaceBrowser_PyType =
{
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    "ifaddrs4py.extern.InterfaceBrowser", // .tp_name
    sizeof(InterfaceBrowser_PyObj), // .tp_basicsize
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
#if PY_MINOR_VERSION > 9
    Py_TPFLAGS_METHOD_DESCRIPTOR | Py_TPFLAGS_IMMUTABLETYPE, // .tp_flags
#else
    Py_TPFLAGS_METHOD_DESCRIPTOR, // .tp_flags
#endif
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
#if PY_MINOR_VERSION < 9
    0, // Py_DEPRECATED(3.8) .tp_print
#elif PY_MINOR_VERSION > 11
    0, // .tp_watched added in 3.12
#endif /* PY_MINOR_VERSION < 9 */
};

#ifndef IS_WINDOWS
#pragma GCC diagnostic pop
#endif /* IS_WINDOWS */

static struct PyMethodDef ifaddrs4py_methods [] =
{
    {"get_sample_interface_ipv4_address", extern_get_sample_interface_ipv4_address, METH_NOARGS, NULL},
    {"get_sample_interface_ipv6_address", extern_get_sample_interface_ipv6_address, METH_NOARGS, NULL},
    {"get_sample_interface_scoped_ipv6_address", extern_get_sample_interface_scoped_ipv6_address, METH_NOARGS, NULL},
    {"get_sample_interface", extern_get_sample_interface, METH_NOARGS, NULL},
    {"get_mac_address_data_from_repr", (PyCFunction) extern_get_mac_address_data_from_repr, METH_FASTCALL, NULL},
    {"get_mac_address_repr_from_data", (PyCFunction) extern_get_mac_address_repr_from_data, METH_FASTCALL, NULL},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef ifaddrs4py_module =
{
    PyModuleDef_HEAD_INIT, // .m_base
    "ifaddrs4py.extern", // .m_name, name of module
    NULL, // .m_doc, module documentation, may be NULL
    -1, // .m_size, size of per-interpreter state of the module, or -1 if the module keeps state in global variables.
    ifaddrs4py_methods, // .m_methods
    NULL, // .m_slots
    NULL, // .m_traverse
    NULL, // .m_clear
    NULL, // .m_free
};

#ifndef IS_WINDOWS
#ifndef __clang__
#pragma GCC diagnostic pop
#endif /* __clang__ */
#endif /* IS_WINDOWS */

#if PY_MINOR_VERSION < 9
#ifndef IS_WINDOWS
// On Python 3.8 and non-Windows only, PyMODINIT_FUNC is missing an export specifier, so do this manually
extern "C" __attribute((visibility("default"))) PyObject *
#else
PyMODINIT_FUNC
#endif /* IS_WINDOWS */
#else
PyMODINIT_FUNC
#endif /* PY_MINOR_VERSION < 9 */
PyInit_extern(void)
{
    if (PyType_Ready(&InterfaceBrowser_PyType) != 0)
    {
        return NULL;
    }

    PyObject * module = PyModule_Create(&ifaddrs4py_module);
    if (module == NULL)
    {
        return NULL;
    }

    try
    {
        OddSource::ifaddrs4py::init_version_constants(module);
        OddSource::ifaddrs4py::init_interface_enums(module);
        OddSource::ifaddrs4py::init_ip_address_samples(module);
        OddSource::ifaddrs4py::init_mac_address_sample(module);
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_ImportError, { Py_DECREF(module); return NULL; })

    IllegalStateError = PyErr_NewException("ifaddrs4py.extern.IllegalStateError", PyExc_RuntimeError, NULL);
    if (IllegalStateError == NULL)
    {
        Py_DECREF(module);
        return NULL;
    }
    Py_INCREF(IllegalStateError);
    if (PyModule_AddObject(module, "IllegalStateError", IllegalStateError) != 0)
    {
        Py_DECREF(IllegalStateError);
        Py_DECREF(module);
        return NULL;
    }

    Py_INCREF(&InterfaceBrowser_PyType);
    if (PyModule_AddObject(module, "InterfaceBrowser", (PyObject *)&InterfaceBrowser_PyType) != 0)
    {
        Py_DECREF(IllegalStateError);
        Py_DECREF(&InterfaceBrowser_PyType);
        Py_DECREF(module);
        return NULL;
    }

    return module;
}
