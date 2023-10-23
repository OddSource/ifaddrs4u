#include "interface.h"
#include "ip_address.h"
#include "mac_address.h"
#include "macros.h"

#include <ifaddrs4cpp/Interface.h>

namespace
{
    template<typename F>
    PyObject * make_enum_class(PyObject * enum_module,
                               char const * enum_name,
                               ::std::unordered_map<::std::string, F const> const & values)
    {
        PyObject * enum_class = NULL;

        {
            PyObject * constants_dict = PyDict_New();
            for (auto const & [name, value] : values)
            {
                PyDict_SetItemString(constants_dict, name.c_str(),  PyLong_FromLong(value));
            }

            enum_class = PyObject_CallMethod(enum_module,
                                             "IntEnum",
                                             "sO",
                                             enum_name,
                                             constants_dict);
            Py_CLEAR(constants_dict);
        }

        if (enum_class == NULL)
        {
            throw ::std::runtime_error("Failed to define new IntEnum class");
        }

        return enum_class;
    }
}

template<class IPAddressT>
PyObject *
OddSource::ifaddrs4py::
convert_to_python(OddSource::Interfaces::InterfaceIPAddress<IPAddressT> const & interface_address)
{
    PyObject * address = NULL, * broadcast_address = NULL, * point_to_point_destination = NULL,
             * flags = NULL, * prefix_length = NULL;
    PyObject * InterfaceIPAddress = NULL;

    try
    {
        address = convert_to_python(interface_address.address());
        broadcast_address = interface_address.broadcast_address() ?
            convert_to_python(*interface_address.broadcast_address()) :
            Py_None;
        point_to_point_destination = interface_address.point_to_point_destination() ?
            convert_to_python(*interface_address.point_to_point_destination()) :
            Py_None;

        flags = PyLong_FromUnsignedLong(interface_address.flags());
        if (flags == NULL)
        {
            throw ::std::runtime_error("Failed to create flags long");
        }

        prefix_length = interface_address.prefix_length() ?
            PyLong_FromUnsignedLong(*interface_address.prefix_length()) :
            Py_None;
        if (prefix_length == NULL)
        {
            throw ::std::runtime_error("Failed to create prefix_length long");
        }

        InterfaceIPAddress = get_module_class("ifaddrs4py.interface", "InterfaceIPAddress");
    }
    catch (...)
    {
        Py_XDECREF(address);
        Py_XDECREF(broadcast_address);
        Py_XDECREF(point_to_point_destination);
        throw;
    }

    PyObject * args(PyTuple_New(0));
    PyObject * kwargs(Py_BuildValue(
        "{s:O,s:O,s:O,s:O,s:O}",
        "address", address,
        "flags", flags,
        "prefix_length", prefix_length,
        "broadcast_address", broadcast_address,
        "point_to_point_destination", point_to_point_destination));
    Py_DECREF(address);
    Py_XDECREF(broadcast_address);
    Py_XDECREF(point_to_point_destination);
    if (args == NULL || kwargs == NULL)
    {
        Py_DECREF(InterfaceIPAddress);
        Py_XDECREF(args);
        Py_XDECREF(kwargs);
        throw ::std::runtime_error("Unable to create args or kwargs");
    }

    PyObject * instance = PyObject_Call(InterfaceIPAddress, args, kwargs);
    Py_DECREF(InterfaceIPAddress);
    Py_DECREF(args);
    Py_DECREF(kwargs);
    if (instance == NULL)
    {
        throw ::std::runtime_error("Unable to instantiate ifaddrs4py.interface.InterfaceIPAddress class");
    }

    return instance;
}

PyObject *
OddSource::ifaddrs4py::
convert_to_python(OddSource::Interfaces::Interface const & interface)
{
    ::std::string name(interface.name());
#ifdef IS_WINDOWS
    ::std::string windows_uuid(interface.windows_uuid());
#endif /* IS_WINDOWS */

    PyObject * index = NULL, * flags = NULL, * mtu = NULL, * mac_address = NULL,
             * ipv4_addresses = NULL, * ipv6_addresses = NULL;
    PyObject * Interface = NULL;

    try
    {
        index = PyLong_FromUnsignedLong(interface.index());
        if (index == NULL)
        {
            throw ::std::runtime_error("Failed to create index long");
        }

        flags = PyLong_FromUnsignedLong(interface.flags());
        if (flags == NULL)
        {
            throw ::std::runtime_error("Failed to create flags long");
        }

        if (interface.mtu())
        {
            mtu = PyLong_FromUnsignedLongLong(*interface.mtu());
            if (mtu == NULL)
            {
                throw ::std::runtime_error("Unable to create MTU long");
            }
        }
        else
        {
            mtu = Py_None;
        }

        mac_address = interface.mac_address() ?
            convert_to_python(*interface.mac_address()) :
            Py_None;

        ipv4_addresses = PyTuple_New(interface.ipv4_addresses().size());
        if (ipv4_addresses == NULL)
        {
            throw ::std::runtime_error("Unable to create tuple of IPv4 interface addresses");
        }
        int i(0);
        for (auto const & ipv4_address : interface.ipv4_addresses())
        {
            PyObject * item = convert_to_python(ipv4_address);
            if (PyTuple_SetItem(ipv4_addresses, i++, item) != 0)
            {
                Py_XDECREF(item);
                throw ::std::runtime_error("Unable to place IPv4 address into tuple");
            }
        }

        ipv6_addresses = PyTuple_New(interface.ipv6_addresses().size());
        if (ipv6_addresses == NULL)
        {
            throw ::std::runtime_error("Unable to create tuple of IPv6 interface addresses");
        }
        i = 0;
        for (auto const & ipv6_address : interface.ipv6_addresses())
        {
            PyObject * item = convert_to_python(ipv6_address);
            if (PyTuple_SetItem(ipv6_addresses, i++, item) != 0)
            {
                Py_XDECREF(item);
                throw ::std::runtime_error("Unable to place IPv6 address into tuple");
            }
        }

        Interface = get_module_class("ifaddrs4py.interface", "Interface");
    }
    catch (...)
    {
        Py_XDECREF(index);
        Py_XDECREF(flags);
        Py_XDECREF(mtu);
        Py_XDECREF(mac_address);
        Py_XDECREF(ipv4_addresses);
        Py_XDECREF(ipv6_addresses);
        throw;
    }

    PyObject * args(PyTuple_New(0));
    PyObject * kwargs(Py_BuildValue(
#ifdef IS_WINDOWS
        "{s:O,s:s#,s:s#,s:O,s:O,s:O,s:O,s:O}",
#else /* IS_WINDOWS */
        "{s:O,s:s#,s:O,s:O,s:O,s:O,s:O}",
#endif /* !IS_WINDOWS */
        "index", index,
        "name", name.c_str(), name.length(),
#ifdef IS_WINDOWS
        "windows_uuid", windows_uuid.c_str(), windows_uuid.length(),
#endif /* IS_WINDOWS */
        "flags", flags,
        "mtu", mtu,
        "mac_address", mac_address,
        "ipv4_addresses", ipv4_addresses,
        "ipv6_addresses", ipv6_addresses));
    Py_XDECREF(mtu);
    Py_XDECREF(mac_address);
    Py_XDECREF(ipv4_addresses);
    Py_XDECREF(ipv6_addresses);
    if (args == NULL || kwargs == NULL)
    {
        Py_DECREF(Interface);
        Py_XDECREF(args);
        Py_XDECREF(kwargs);
        throw ::std::runtime_error("Unable to create args or kwargs");
    }

    PyObject * instance = PyObject_Call(Interface, args, kwargs);
    Py_DECREF(Interface);
    Py_DECREF(args);
    Py_DECREF(kwargs);
    if (instance == NULL)
    {
        throw ::std::runtime_error("Unable to instantiate ifaddrs4py.interface.Interface class");
    }

    return instance;
}

PyObject *
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

PyObject *
extern_get_sample_interface_ipv6_address(PyObject * module_self, PyObject * Py_UNUSED(ignore))
{
    static OddSource::Interfaces::InterfaceIPv6Address const IPv6(
        OddSource::Interfaces::IPv6Address("2001:470:2ccb:a61b:e:acf8:6736:d81e"),
        OddSource::Interfaces::AutoConfigured | OddSource::Interfaces::Secured,
        56u);

    try
    {
        return OddSource::ifaddrs4py::convert_to_python(IPv6);
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_RuntimeError, return NULL)
}

PyObject *
extern_get_sample_interface_scoped_ipv6_address(PyObject * module_self, PyObject * Py_UNUSED(ignore))
{
    using namespace OddSource::Interfaces;
    static InterfaceIPv6Address const Scoped_IPv6(
        IPv6Address(static_cast<in6_addr const *>(IPv6Address("fe80::aede:48ff:fe00:1122")), v6Scope {6, "en5"}),
        Secured,
        64u);

    try
    {
        return OddSource::ifaddrs4py::convert_to_python(Scoped_IPv6);
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_RuntimeError, return NULL)
}

PyObject *
extern_get_sample_interface(PyObject * module_self, PyObject * Py_UNUSED(ignore))
{
    try
    {
        return OddSource::ifaddrs4py::convert_to_python(OddSource::Interfaces::Interface::SAMPLE_INTERFACE);
    }
    CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(PyExc_RuntimeError, return NULL)
}

void
OddSource::ifaddrs4py::
init_interface_enums(PyObject * module)
{
    PyObject * enum_module = get_module("enum");

    PyObject * enum_class;
    int result;

    using namespace OddSource::Interfaces;

    enum_class = make_enum_class<InterfaceIPAddressFlag>(enum_module,
                                                         "InterfaceIPAddressFlag",
                                                         InterfaceIPAddressFlag_Values);
    result = PyModule_AddObjectRef(module, "InterfaceIPAddressFlag", enum_class);
    Py_XDECREF(enum_class);
    if (result != 0)
    {
        throw ::std::runtime_error("Failed to add enum to module");
    }

    enum_class = make_enum_class<InterfaceFlag>(enum_module,
                                                "InterfaceFlag",
                                                InterfaceFlag_Values);
    result = PyModule_AddObjectRef(module, "InterfaceFlag", enum_class);
    Py_XDECREF(enum_class);
    if (result != 0)
    {
        throw ::std::runtime_error("Failed to add enum to module");
    }
}
