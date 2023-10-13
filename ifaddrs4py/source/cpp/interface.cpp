#include "interface.h"

#include "ifaddrs4cpp/Interface.h"

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

void
OddSource::ifaddrs4py::
init_interface_enums(PyObject * module)
{
    PyObject * enum_module = PyImport_ImportModule("enum");
    if (enum_module == NULL)
    {
        throw ::std::runtime_error("Failed to import enum");
    }

    PyObject * enum_class;
    int result;

    using namespace OddSource::Interfaces;

    enum_class = make_enum_class<InterfaceIPAddressFlag>(enum_module,
                                                         "InterfaceIPAddressFlag",
                                                         InterfaceIPAddressFlag_Values);
    result = PyModule_AddObjectRef(module, "InterfaceIPAddressFlag", enum_class);
    Py_XDECREF(enum_class);
    if (result)
    {
        throw ::std::runtime_error("Failed to add enum to module");
    }

    enum_class = make_enum_class<InterfaceFlag>(enum_module,
                                                "InterfaceFlag",
                                                InterfaceFlag_Values);
    result = PyModule_AddObjectRef(module, "InterfaceFlag", enum_class);
    Py_XDECREF(enum_class);
    if (result)
    {
        throw ::std::runtime_error("Failed to add enum to module");
    }
}