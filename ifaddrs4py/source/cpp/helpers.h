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

#include "config.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace OddSource::ifaddrs4py
{
    class PyRef
    {
    public:
        PyRef(PyObject * object);

        ~PyRef();

        int gc_visit(visitproc visit, void * arg);

        PyObject * ref;
    };

    class ImportedModule
    {
    public:
        ImportedModule(PyObject * module);

        ~ImportedModule() = default;

        ::std::shared_ptr<PyRef> get_attribute(const char * attr_name);

        int gc_visit(visitproc visit, void * arg);

    private:
        ::std::shared_ptr<PyRef> _module;
        ::std::mutex _attrs_mutex;
        ::std::unordered_map<::std::string, ::std::shared_ptr<PyRef>> _attrs;
    };

    class ModuleState
    {
    public:
        ModuleState();

        ~ModuleState() = default;

        static ::std::shared_ptr<ModuleState> get_state_of(PyTypeObject * type);

        static ::std::shared_ptr<ModuleState> get_state_of(PyObject * module);

        ::std::shared_ptr<ImportedModule> get_module(const char * module_name);

        int gc_visit(visitproc visit, void * arg);

    private:
        ::std::mutex _modules_mutex;
        ::std::unordered_map<::std::string, ::std::shared_ptr<ImportedModule>> _modules;
    };

    struct ModuleStateWrapper
    {
        ::std::shared_ptr<ModuleState> state = nullptr;
    };
}