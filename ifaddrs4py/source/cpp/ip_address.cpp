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

#include <ifaddrs4cpp/IpAddress.h>

PyObject *
OddSource::ifaddrs4py::
convert_to_python(OddSource::Interfaces::IPv4Address const & address)
{
    PyObject * IPv4Address(get_module_class("ifaddrs4py.ip_address", "IPv4Address"));

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
        throw ::std::runtime_error("Unable to instantiate ifaddrs4py.ip_address.IPv4Address class");
    }

    return instance;
}

PyObject *
OddSource::ifaddrs4py::
convert_to_python(OddSource::Interfaces::IPv6Address const & address)
{
    PyObject * IPv6Address(get_module_class("ifaddrs4py.ip_address", "IPv6Address"));

    auto addr = static_cast< in6_addr const * >(address);
    auto bytes = reinterpret_cast< uint8_t const * >(addr);

    PyObject * args(Py_BuildValue("(y#)", bytes, 16));
    if (args == NULL)
    {
        Py_DECREF(IPv6Address);
        throw ::std::runtime_error("Unable to create args list");
    }

    PyObject * kwargs(NULL);
    if (address.has_scope_id())
    {
        PyObject * scope_id_py(Py_None);
        if (address.scope_id())
        {
            scope_id_py = PyLong_FromUnsignedLong(*address.scope_id());
        }

        PyObject * scope_name_py(Py_None);
        if (address.scope_name())
        {
            ::std::string scope_name(*address.scope_name());
            scope_name_py = PyUnicode_FromStringAndSize(scope_name.c_str(), scope_name.length());
        }

        kwargs = Py_BuildValue("{s:O,s:O}", "scope_id", scope_name_py, "scope_number", scope_id_py);
        Py_XDECREF(scope_name_py);
        Py_XDECREF(scope_id_py);
        if (kwargs == NULL)
        {
            Py_DECREF(IPv6Address);
            Py_DECREF(args);
            throw ::std::runtime_error("Unable to create kwargs dict");
        }
    }

    PyObject * instance = PyObject_Call(IPv6Address, args, kwargs);
    Py_DECREF(IPv6Address);
    Py_DECREF(args);
    Py_XDECREF(kwargs);
    if (instance == NULL)
    {
        throw ::std::runtime_error("Unable to instantiate ifaddrs4py.ip_address.IPv6Address class");
    }

    return instance;
}

void
OddSource::ifaddrs4py::
init_ip_address_samples(PyObject * module)
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