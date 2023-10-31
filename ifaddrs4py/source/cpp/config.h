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

#pragma once

#define PY_SSIZE_T_CLEAN  /* Make "s#" use Py_ssize_t rather than int. */
#include <Python.h>

#include <stdexcept>
#include <string>

using std::string_literals::operator""s;

#if PY_MAJOR_VERSION != 3 || PY_MINOR_VERSION < 8
#error "ifaddrs4py requires Python 3.8+"
#endif /* PY_MAJOR_VERSION != 3 || PY_MINOR_VERSION < 8 */

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#define IS_WINDOWS 1
#endif

namespace OddSource::ifaddrs4py
{
    inline PyObject * get_module(char const * module_name)
    {
        PyObject * module = PyImport_ImportModule(module_name);
        if (module == NULL)
        {
            throw ::std::runtime_error(
                "Failed to import module '"s + module_name + "'"s);
        }
        return module;
    }

    inline PyObject * get_module_class(char const * module_name, char const * class_name)
    {
        PyObject * module = get_module(module_name);

        PyObject * klass = PyObject_GetAttrString(module, class_name);
        Py_DECREF(module);
        if (klass == NULL)
        {
            throw ::std::runtime_error(
                "Failed to retrieve class '"s + class_name + "' from module '"s + module_name + "'"s);
        }

        return klass;
    }
}
