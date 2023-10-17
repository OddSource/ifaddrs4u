#include "interface.h"
#include "ip_address.h"

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

PyObject *
OddSource::ifaddrs4py::
convert_to_python(OddSource::Interfaces::MacAddress const & address)
{
    PyObject * MacAddress(get_module_class("ifaddrs4py.mac_address", "MacAddress"));

    ::std::string repr(address);
    auto data = static_cast<uint8_t const *>(address);
    auto data_length = address.length();

    PyObject * tuple = PyTuple_New(data_length);
    if(tuple == NULL)
    {
        Py_DECREF(MacAddress);
        throw ::std::runtime_error("Unable to create tuple");
    }
    for(Py_ssize_t i = 0; i < data_length; i++)
    {
        PyObject * item = PyLong_FromUnsignedLong(data[i]);
        if(item == NULL)
        {
           Py_DECREF(MacAddress);
           Py_DECREF(tuple);
           throw ::std::runtime_error("Unable to create long");
        }

        if (PyTuple_SetItem(tuple, i, item) != 0)
        {
            Py_XDECREF(item);
            throw ::std::runtime_error("Unable to place long in tuple");
        }
    }

    PyObject * args(Py_BuildValue("(s#O)", repr.c_str(), repr.length(), tuple));
    Py_DECREF(tuple);
    if (args == NULL)
    {
        Py_DECREF(MacAddress);
        throw ::std::runtime_error("Unable to create args list");
    }

    PyObject * instance = PyObject_Call(MacAddress, args, NULL);
    Py_DECREF(MacAddress);
    Py_DECREF(args);
    if (instance == NULL)
    {
        throw ::std::runtime_error("Unable to instantiate ifaddrs4py.mac_address.MacAddress class");
    }

    return instance;
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

    static OddSource::Interfaces::MacAddress const MAC("ac:de:48:00:11:22");

    PyObject * mac = convert_to_python(MAC);
    result = PyModule_AddObjectRef(module, "_TEST_MAC_ADDRESS", mac);
    Py_DECREF(mac);
    if (result != 0)
    {
        throw ::std::runtime_error("Unable to initialize _TEST_MAC_ADDRESS.");
    }
}
