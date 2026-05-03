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

#include "config.h"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace OddSource::ifaddrs4py
{
    template< class PyT = PyObject >
    class PyRef
    {
    public:
        class IncreasedReference
        {
        public:
            IncreasedReference(
                PyT * object ) noexcept;

            ~IncreasedReference() noexcept;

        private:
            PyT * _ref;
        };

        PyRef(
            PyT * object ) noexcept;

        ~PyRef() noexcept;

        operator PyT *() noexcept;

        IncreasedReference
        incRef() noexcept;

        int
        gcVisit(
            visitproc visit,
            void * arg );

    private:
        PyT * _ref;
    };

    class ImportedModule
    {
    public:
        ImportedModule(
            PyObject * module );

        ~ImportedModule() = default;

        ::std::shared_ptr< PyRef<> >
        getAttribute(
            char const * attrName );

        int
        gcVisit(
            visitproc visit,
            void * arg );

    private:
        ::std::shared_ptr< PyRef<> > _module;
        ::std::mutex _attrsMutex;
        ::std::unordered_map< ::std::string, ::std::shared_ptr< PyRef<> > > _attrs;
    };

    class ModuleState
    {
    public:
        ModuleState();

        ~ModuleState() = default;

        static
        ::std::shared_ptr< ModuleState >
        getStateOf(
            PyTypeObject * type );

        static
        ::std::shared_ptr< ModuleState >
        getStateOf(
            PyObject * module );

        ::std::shared_ptr< ImportedModule >
        getModule(
            char const * moduleName );

        int
        gcVisit(
            visitproc visit,
            void * arg );

    private:
        ::std::mutex _modulesMutex;
        ::std::unordered_map< ::std::string, ::std::shared_ptr< ImportedModule > > _modules;
    };

    struct ModuleStateWrapper
    {
        ::std::shared_ptr< ModuleState > state = nullptr;
    };
}