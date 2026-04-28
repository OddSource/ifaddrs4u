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

#include "helpers.h"
#include "macros.h"

namespace OddSource::ifaddrs4py
{
    PyRef::
    PyRef(PyObject * object)
        : ref(object)
    {
    }

    PyRef::
    ~PyRef()
    {
        Py_XDECREF(this->ref);
    }

    int
    PyRef::
    gc_visit(visitproc visit, void * arg)
    {
        Py_VISIT(this->ref);
    }

    ImportedModule::
    ImportedModule(PyObject * module)
        : _module(::std::make_shared<PyRef>(module)),
          _attrs_mutex(),
          _attrs()
    {
    }

    ::std::shared_ptr<PyRef>
    ImportedModule::
    get_attribute(const char * attr_name)
    {
        ::std::unique_lock<::std::mutex> lock(this->_attrs_mutex);
        auto found(this->_attrs.find(attr_name));
        if (found == this->_attrs.end())
        {
            PyObject * attr = PyObject_GetAttrString(this->_module->ref, attr_name);
            IF_NULL_RETURN(module, nullptr)

            this->_attrs.emplace(attr_name, ::std::make_shared<PyRef>(attr));
            found = this->_attrs.find(attr_name);
        }

        return found->second;
    }

    int
    ImportedModule::
    gc_visit(visitproc visit, void * arg)
    {
        ::std::unique_lock<::std::mutex> lock(this->_attrs_mutex);
        for (auto & [name, attr] : this->_attrs)
        {
            ::std::ignore = name;
            auto result(attr->gc_visit(visit, arg));
            if (result != 0)
            {
                return result;
            }
        }
    }

    ModuleState::
    ModuleState()
        : _modules_mutex(),
          _modules()
    {
    }

    namespace
    {
        inline ::std::shared_ptr<ModuleState>
        state_from_state_wrapper(void * wrapper)
        {
            return reinterpret_cast<ModuleStateWrapper *>(wrapper)->state;
        }
    }

    ::std::shared_ptr<ModuleState>
    ModuleState::
    get_state_of(PyTypeObject * module)
    {
        void * state_wrapper(PyType_GetModuleState(module));
        IF_NULL_RETURN_NULL(state)
        return state_from_state_wrapper(state_wrapper);
    }

    ::std::shared_ptr<ModuleState>
    ModuleState::
    get_state_of(PyObject * module)
    {
        void * state_wrapper(PyModule_GetState(module));
        IF_NULL_RETURN_NULL(state)
        return state_from_state_wrapper(state_wrapper);
    }

    ::std::shared_ptr<ImportedModule>
    ModuleState::
    get_module(const char * module_name)
    {
        ::std::unique_lock<::std::mutex> lock(this->_modules_mutex);
        auto found(this->_modules.find(module_name));
        if (found == this->_modules.end())
        {
            PyObject * module(PyImport_ImportModule(module_name));
            IF_NULL_RETURN(module, nullptr)

            this->_modules.emplace(module_name, ::std::make_shared<ImportedModule>(module));
            found = this->_modules.find(module_name);
        }

        return found->second;
    }

    int
    ImportedModule::
    gc_visit(visitproc visit, void * arg)
    {
        ::std::unique_lock<::std::mutex> lock(this->_modules_mutex);
        for (auto & [name, module] : this->_modules)
        {
            ::std::ignore = name;
            auto result(module->gc_visit(visit, arg));
            if (result != 0)
            {
                return result;
            }
        }
    }
}
