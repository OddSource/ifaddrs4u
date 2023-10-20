#include "ip_address.h"

#include <ifaddrs4cpp/IpAddress.h>

PyObject *
OddSource::ifaddrs4py::
convert_to_python(OddSource::Interfaces::IPv4Address const & address)
{
    PyObject * IPv4Address(get_module_class("ipaddress", "IPv4Address"));

    auto addr = static_cast< in_addr const * >(address);
    auto ints = reinterpret_cast< uint32_t const * >(addr);

    PyObject * args(Py_BuildValue("(I)", ntohl(*ints)));
    if (args == NULL)
    {
        Py_DECREF(IPv4Address);
        throw ::std::runtime_error("Unable to create args list");
    }

    PyObject * instance = PyObject_Call(IPv4Address, args, NULL);
    Py_DECREF(IPv4Address);
    Py_DECREF(args);
    if (instance == NULL)
    {
        throw ::std::runtime_error("Unable to instantiate ipaddress.IPv4Address class");
    }

    return instance;
}

PyObject *
OddSource::ifaddrs4py::
convert_to_python(OddSource::Interfaces::IPv6Address const & address)
{
    PyObject * IPv6Address(get_module_class("ipaddress", "IPv6Address"));

    auto addr = static_cast< in6_addr const * >(address);
    auto bytes = reinterpret_cast< uint8_t const * >(addr);

    PyObject * args(Py_BuildValue("(y#)", bytes, 16));
    if (args == NULL)
    {
        Py_DECREF(IPv6Address);
        throw ::std::runtime_error("Unable to create args list");
    }

    PyObject * instance = PyObject_Call(IPv6Address, args, NULL);
    Py_DECREF(IPv6Address);
    Py_DECREF(args);
    if (instance == NULL)
    {
        throw ::std::runtime_error("Unable to instantiate ipaddress.IPv6Address class");
    }

    if (address.has_scope_id())
    {
        ::std::string scope_id(*address.scope_name_or_id());
        PyObject * scope_id_py(PyUnicode_FromStringAndSize(scope_id.c_str(), scope_id.length()));
        if (scope_id_py == NULL)
        {
            throw ::std::runtime_error("Unable to convert scope_id to PyUnicode");
        }
        if (PyObject_SetAttrString(instance, "_scope_id", scope_id_py) != 0)
        {
            ::std::string warning("Unable to set IPv6Address._scope_id, so the API might be different on this platform.");

#if PY_MINOR_VERSION < 12
            PyObject * e_type, * e_value, * e_traceback;
            PyErr_Fetch(&e_type, &e_value, &e_traceback);
#else /* PY_MINOR_VERSION < 12 */
            PyObject * e_value = PyErr_GetRaisedException();
#endif /* PY_MINOR_VERSION >= 12 */

            if (e_value != NULL)
            {
                PyObject * err_string = PyObject_Str(e_value);
                PyErr_Clear();
                if (err_string != NULL)
                {
                    char const * more(PyUnicode_AsUTF8(err_string));
                    PyErr_Clear();
                    if (more)
                    {
                        warning += " More information: ";
                        warning += more;
                    }
                }
                Py_DECREF(e_value);
#if PY_MINOR_VERSION < 12
                Py_DECREF(e_type);
                Py_DECREF(e_traceback);
#endif /* PY_MINOR_VERSION < 12 */
            }

            PyErr_WarnEx(PyExc_RuntimeWarning, warning.c_str(), 1);
        }
    }

    return instance;
}

void
OddSource::ifaddrs4py::
init_test_ip_address_constants(PyObject * module)
{
    static OddSource::Interfaces::IPv4Address const LO_V4("127.0.0.1");
    static OddSource::Interfaces::IPv6Address const LO_V6("::1");

    PyObject * lo = convert_to_python(LO_V4);
    int result = PyModule_AddObjectRef(module, "_TEST_LOOPBACK_V4", lo);
    Py_DECREF(lo);
    if (result != 0)
    {
        throw ::std::runtime_error("Unable to initialize _TEST_LOOPBACK_V4.");
    }

    lo = convert_to_python(LO_V6);
    result = PyModule_AddObjectRef(module, "_TEST_LOOPBACK_V6", lo);
    Py_DECREF(lo);
    if (result != 0)
    {
        throw ::std::runtime_error("Unable to initialize _TEST_LOOPBACK_V6.");
    }
}