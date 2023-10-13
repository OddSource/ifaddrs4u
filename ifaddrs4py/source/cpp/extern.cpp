#include "version.h"
#include "interface.h"

#include <stdexcept>

static PyMethodDef ifaddrs4py_methods [] = {
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef ifaddrs4py_module = {
    PyModuleDef_HEAD_INIT,
    "ifaddrs4py.extern",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1, /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    ifaddrs4py_methods
};

PyMODINIT_FUNC
PyInit_extern(void)
{
    PyObject * module = NULL;
    module = PyModule_Create(&ifaddrs4py_module);
    if (module == NULL)
    {
        Py_XDECREF(module);
        return NULL;
    }

    try
    {
        OddSource::ifaddrs4py::init_version_constants(module);
        OddSource::ifaddrs4py::init_interface_enums(module);
    }
    catch(::std::runtime_error const &)
    {
        Py_XDECREF(module);
        return NULL;
    }

    return module;
}
