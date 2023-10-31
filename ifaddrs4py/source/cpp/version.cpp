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

#include "version.h"

#include <stdexcept>
#include <string>

#include <ifaddrs4cpp/config.h>

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

    if (PyModule_AddStringConstant(module, "__VERSION_EXT__", version.c_str()) != 0)
    {
        throw ::std::runtime_error("Failed to initialized __VERSION_EXT__");
    }

    if (l > 0 && PyModule_AddObject(
        module,
        "__VERSION_INFO_EXT__",
        Py_BuildValue("iiis", IFADDRS4CPP_VERSION_MAJOR,
                              IFADDRS4CPP_VERSION_MINOR,
                              IFADDRS4CPP_VERSION_PATCH,
                              IFADDRS4CPP_VERSION_SUFFIX)) != 0)
    {
        throw ::std::runtime_error("Failed to initialized __VERSION_INFO_EXT__");
    }
    else if(l == 0 && PyModule_AddObject(
        module,
        "__VERSION_INFO_EXT__",
        Py_BuildValue("iii", IFADDRS4CPP_VERSION_MAJOR,
                              IFADDRS4CPP_VERSION_MINOR,
                              IFADDRS4CPP_VERSION_PATCH)) != 0)
    {
        throw ::std::runtime_error("Failed to initialized __VERSION_INFO_EXT__");
    }
}
