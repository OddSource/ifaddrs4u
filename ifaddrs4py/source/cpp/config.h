/*
 * Copyright © 2010-2026 OddSource Code (license@oddsource.io)
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

#pragma once

#define PY_SSIZE_T_CLEAN  /* Make "s#" use Py_ssize_t rather than int. */
#include <Python.h>

#include <stdexcept>
#include <string>

#if PY_MAJOR_VERSION != 3 || PY_MINOR_VERSION < 10
#error "ifaddrs4py requires Python 3.10+"
#endif /* PY_MAJOR_VERSION != 3 || PY_MINOR_VERSION < 10 */

#include <oddsource/network/interfaces/detail/os.h>

using namespace ::std::string_literals;

namespace OddSource::ifaddrs4py
{
    inline
    PyObject *
    getModule(
        char const * moduleName )
    {
        PyObject * module = PyImport_ImportModule( moduleName );
        if ( module == NULL )
        {
            throw ::std::runtime_error(
                "Failed to import module '"s + moduleName + "'"s );
        }
        return module;
    }

    inline
    PyObject *
    getModuleClass(
        char const * moduleName,
        char const * className )
    {
        PyObject * module = getModule( moduleName );

        PyObject * klass = PyObject_GetAttrString( module, className );
        Py_DECREF( module );
        if ( klass == NULL )
        {
            throw ::std::runtime_error(
                "Failed to retrieve class '"s + className + "' from module '"s + moduleName + "'"s );
        }

        return klass;
    }
}
