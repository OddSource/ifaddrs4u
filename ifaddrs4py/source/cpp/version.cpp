#include "version.h"

#include <stdexcept>
#include <string>

#include "ifaddrs4cpp/config.h"

void
OddSource::ifaddrs4py::
init_version_constants(PyObject * module)
{
    auto l(::strlen(IFADDRS4CPP_VERSION_SUFFIX));
    ::std::string version(IFADDRS4CPP_VERSION);
    if (l > 0)
    {
        version += "-";
        version += IFADDRS4CPP_VERSION_SUFFIX;
    }

    if (PyModule_AddStringConstant(module, "__VERSION_EXT__", version.c_str()))
    {
        throw ::std::runtime_error("Failed to initialized __VERSION_EXT__");
    }

    if (l > 0 && PyModule_AddObject(
        module,
        "__VERSION_INFO_EXT__",
        Py_BuildValue("iiis", IFADDRS4CPP_VERSION_MAJOR,
                              IFADDRS4CPP_VERSION_MINOR,
                              IFADDRS4CPP_VERSION_PATCH,
                              IFADDRS4CPP_VERSION_SUFFIX)))
    {
        throw ::std::runtime_error("Failed to initialized __VERSION_INFO_EXT__");
    }
    else if(l == 0 && PyModule_AddObject(
        module,
        "__VERSION_INFO_EXT__",
        Py_BuildValue("iii", IFADDRS4CPP_VERSION_MAJOR,
                              IFADDRS4CPP_VERSION_MINOR,
                              IFADDRS4CPP_VERSION_PATCH)))
    {
        throw ::std::runtime_error("Failed to initialized __VERSION_INFO_EXT__");
    }
}
