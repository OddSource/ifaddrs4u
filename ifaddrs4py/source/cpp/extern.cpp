#include "config.h"
#include "interface.h"
#include "ip_address.h"
#include "version.h"

#include "structmember.h"  // not include in Python.h

#include <sstream>
#include <stdexcept>

#include <ifaddrs4cpp/Interfaces.h>

static PyObject * IllegalStateError;

typedef struct
{
    PyObject_HEAD
    ::std::unique_ptr<OddSource::Interfaces::InterfaceBrowser> browser;
    PyTupleObject * interfaces;  // tuple
} InterfaceBrowser_PyObj;

#define CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(err, ret) catch (::std::exception const & e) \
    { \
        if (PyErr_Occurred() == NULL) \
        { \
            PyErr_SetString(err, e.what()); \
        } \
        ret; \
    }

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
        for (auto const & interface : interfaces)
        {
            try
            {
                PyObject * item = OddSource::ifaddrs4py::convert_to_python(interface);
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

    PyObject * callable(args[0]);
    if (!PyCallable_Check(callable))
    {
        ::std::ostringstream oss;
        oss << "InterfaceBrowser.for_each_interface() expected second argument to be callable, not "
            << callable->ob_type->tp_name << ".";
        PyErr_SetString(PyExc_ValueError, oss.str().c_str());
        return NULL;
    }

    ENSURE_INITIALIZED(NULL)

    bool keep_calling_callable(true);
    bool return_error(false);

    PyObject * temp_list = NULL;
    if (self->interfaces == NULL)
    {
        temp_list = PyList_New(0);
        if (temp_list == NULL)
        {
            return NULL;
        }
    }

    ::std::function<bool(Interface const &)> do_this =
    [callable, &keep_calling_callable, &return_error, &temp_list](auto interface)
    {
        PyObject * item;
        try
        {
            item = OddSource::ifaddrs4py::convert_to_python(interface);
        }
        CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_RuntimeError, return_error = true)

        if (!return_error && keep_calling_callable)
        {
            PyObject * result = PyObject_CallFunctionObjArgs(callable, item, NULL);
            if (result == NULL)
            {
                return_error = true;
            }
            else if (!PyBool_Check(result))
            {
                ::std::ostringstream oss;
                oss << "InterfaceBrowser.for_each_interface() expected second argument to be a callable that returns "
                    << "a bool, but instead it returned a " << result->ob_type->tp_name << ".";
                PyErr_SetString(PyExc_ValueError, oss.str().c_str());
                return_error = true;
            }
            else
            {
                keep_calling_callable = PyObject_IsTrue(result) ? true : false;
            }
            Py_DECREF(result);
        }

        if (!return_error && temp_list != NULL && PyList_Append(temp_list, item) != 0)
        {
            return_error = true;
        }

        Py_DECREF(item);
        return !return_error;
    };

    self->browser->for_each_interface(do_this);

    if (!return_error && temp_list != NULL && self->interfaces == NULL)
    {
        self->interfaces = (PyTupleObject *)PyList_AsTuple(temp_list);
        if (self->interfaces == NULL)
        {
            return_error = true;
        }
    }

    Py_XDECREF(temp_list);

    if (return_error)
    {
        return NULL;
    }

    Py_RETURN_NONE;
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
InterfaceBrowser_get_interfaces(InterfaceBrowser_PyObj * self)
{
    ENSURE_INITIALIZED(NULL)
    ENSURE_POPULATED(NULL)
    return (PyObject *)self->interfaces;
}

static PyObject *
InterfaceBrowser___iter__(InterfaceBrowser_PyObj * self, PyObject * Py_UNUSED(ignore))
{
    ENSURE_INITIALIZED(NULL)
    ENSURE_POPULATED(NULL)
    return PyObject_GetIter((PyObject *)self->interfaces);
}

static Py_ssize_t
InterfaceBrowser___len__(InterfaceBrowser_PyObj * self)
{
    ENSURE_INITIALIZED(-1)
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

static struct PyMemberDef InterfaceBrowser_members [] =
{
    {NULL}  /* Sentinel */
};

static struct PyMethodDef InterfaceBrowser_methods [] =
{
    {"get_interfaces", (PyCFunction) InterfaceBrowser_get_interfaces, METH_NOARGS, NULL},
    {"length", (PyCFunction) InterfaceBrowser_length, METH_NOARGS, NULL},
    {"get_interface", (PyCFunction) InterfaceBrowser_get_interface, METH_FASTCALL, NULL},
    {"for_each_interface", (PyCFunction) InterfaceBrowser_for_each_interface, METH_FASTCALL, NULL},
    {NULL}  /* Sentinel */
};

static struct PyGetSetDef InterfaceBrowser_getters_and_setters [] =
{
    {NULL}  /* Sentinel */
};

static PyMappingMethods InterfaceBrowser_mapping =
{
    .mp_length = (lenfunc) InterfaceBrowser___len__,
    .mp_subscript = (binaryfunc) InterfaceBrowser___getitem___mapping,
};

static PyTypeObject InterfaceBrowser_PyType =
{
    PyVarObject_HEAD_INIT(&PyType_Type, 0)
    .tp_name = "ifaddrs4py.extern.InterfaceBrowser",
    .tp_basicsize = sizeof(InterfaceBrowser_PyObj),
    .tp_flags = Py_TPFLAGS_METHOD_DESCRIPTOR | Py_TPFLAGS_IMMUTABLETYPE,
    .tp_new = PyType_GenericNew,
    .tp_init = (initproc) InterfaceBrowser___init__,
    .tp_dealloc = (destructor) InterfaceBrowser_dealloc,
    .tp_repr = (reprfunc) InterfaceBrowser___repr__,
    .tp_str = (reprfunc) InterfaceBrowser___repr__,
    .tp_iter = (getiterfunc) InterfaceBrowser___iter__,
    .tp_members = InterfaceBrowser_members,
    .tp_methods = InterfaceBrowser_methods,
    .tp_getset = InterfaceBrowser_getters_and_setters,
    .tp_as_mapping = &InterfaceBrowser_mapping,
};

static PyObject *
extern_get_sample_interface_ipv4_address(PyObject * module_self, PyObject * Py_UNUSED(ignore))
{
    static OddSource::Interfaces::InterfaceIPv4Address const IPv4(
        OddSource::Interfaces::IPv4Address("192.168.0.42"),
        0,
        24u,
        OddSource::Interfaces::IPv4Address("192.168.0.254"));

    try
    {
        return OddSource::ifaddrs4py::convert_to_python(IPv4);
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_RuntimeError, return NULL)
}

static PyObject *
extern_get_sample_interface_ipv6_address(PyObject * module_self, PyObject * Py_UNUSED(ignore))
{
    static OddSource::Interfaces::InterfaceIPv6Address const IPv6(
        OddSource::Interfaces::IPv6Address("2001:470:2ccb:a61b:e:acf8:6736:d81e"),
        OddSource::Interfaces::AutoConfigured | OddSource::Interfaces::Secured,
        64u);

    try
    {
        return OddSource::ifaddrs4py::convert_to_python(IPv6);
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_RuntimeError, return NULL)
}

static PyObject *
extern_get_sample_interface(PyObject * module_self, PyObject * Py_UNUSED(ignore))
{
    try
    {
        return OddSource::ifaddrs4py::convert_to_python(OddSource::Interfaces::Interface::SAMPLE_INTERFACE);
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_RuntimeError, return NULL)
}

static struct PyMethodDef ifaddrs4py_methods [] =
{
    {"get_sample_interface_ipv4_address", extern_get_sample_interface_ipv4_address, METH_NOARGS, NULL},
    {"get_sample_interface_ipv6_address", extern_get_sample_interface_ipv6_address, METH_NOARGS, NULL},
    {"get_sample_interface", extern_get_sample_interface, METH_NOARGS, NULL},
    {NULL}        /* Sentinel */
};

static struct PyModuleDef ifaddrs4py_module =
{
    PyModuleDef_HEAD_INIT,
    "ifaddrs4py.extern",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    ifaddrs4py_methods
};

PyMODINIT_FUNC
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
        OddSource::ifaddrs4py::init_test_ip_address_constants(module);
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
