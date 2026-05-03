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

#include "helpers.h"
#include "macros.h"

namespace OddSource::ifaddrs4py
{
    template< class PyT >
    PyRef< PyT >::
    PyRef(
        PyT * object ) noexcept
        : _ref( object )
    {
    }

    template< class PyT >
    PyRef< PyT >::
    ~PyRef() noexcept
    {
        Py_XDECREF( this->_ref );
    }

    template< class PyT >
    PyRef< PyT >::
    operator PyT *() noexcept
    {
        return this->_ref;
    }

    template< class PyT >
    typename PyRef< PyT >::IncreasedReference
    PyRef< PyT >::
    incRef() noexcept
    {
        return { this->_ref };
    }

    template< class PyT >
    int
    PyRef< PyT >::
    gcVisit(
        visitproc visit,
        void * arg )
    {
        Py_VISIT( this->_ref );
        return 0;
    }

    template< class PyT >
    PyRef< PyT >::IncreasedReference::
    IncreasedReference(
        PyT * object ) noexcept
        : _ref( object )
    {
        Py_INCREF( this->_ref );
    }

    template< class PyT >
    PyRef< PyT >::IncreasedReference::
    ~IncreasedReference() noexcept
    {
        Py_DECREF( this->_ref );
    }

    ImportedModule::
    ImportedModule(
        PyObject * module )
        : _module( ::std::make_shared< PyRef<> >( module ) ),
          _attrsMutex(),
          _attrs()
    {
    }

    ::std::shared_ptr< PyRef<> >
    ImportedModule::
    getAttribute(
        char const * attrName )
    {
        ::std::unique_lock< ::std::mutex > lock( this->_attrsMutex );
        auto found( this->_attrs.find( attrName ) );
        if ( found == this->_attrs.end() )
        {
            PyObject * attr = PyObject_GetAttrString( *this->_module, attrName );
            IF_NULL_RETURN( attr, nullptr )

            this->_attrs.emplace( attrName, ::std::make_shared< PyRef<> >( attr ) );
            found = this->_attrs.find( attrName );
        }

        return found->second;
    }

    int
    ImportedModule::
    gcVisit(
        visitproc visit,
        void * arg )
    {
        ::std::unique_lock< ::std::mutex > lock( this->_attrsMutex );
        for ( auto & [ name, attr ] : this->_attrs )
        {
            ::std::ignore = name;
            auto const result( attr->gcVisit( visit, arg ) );
            if ( result != 0 )
            {
                return result;
            }
        }
        return 0;
    }

    ModuleState::
    ModuleState()
        : _modulesMutex(),
          _modules()
    {
    }

    namespace
    {
        inline
        ::std::shared_ptr< ModuleState >
        stateFromStateWrapper(
            void * stateWrapper )
        {
            return reinterpret_cast< ModuleStateWrapper * >( stateWrapper )->state;
        }
    }

    ::std::shared_ptr< ModuleState >
    ModuleState::
    getStateOf(
        PyTypeObject * module )
    {
        void * stateWrapper( PyType_GetModuleState( module ) );
        IF_NULL_RETURN_NULL( stateWrapper )
        return stateFromStateWrapper( stateWrapper );
    }

    ::std::shared_ptr< ModuleState >
    ModuleState::
    getStateOf(
        PyObject * module )
    {
        void * stateWrapper( PyModule_GetState( module ) );
        IF_NULL_RETURN_NULL( stateWrapper )
        return stateFromStateWrapper( stateWrapper );
    }

    ::std::shared_ptr< ImportedModule >
    ModuleState::
    getModule(
        char const * moduleName )
    {
        ::std::unique_lock< ::std::mutex > lock( this->_modulesMutex );
        auto found( this->_modules.find( moduleName ) );
        if ( found == this->_modules.end() )
        {
            PyObject * module( PyImport_ImportModule( moduleName ) );
            IF_NULL_RETURN( module, nullptr )

            this->_modules.emplace( moduleName, ::std::make_shared< ImportedModule >( module ) );
            found = this->_modules.find( moduleName );
        }

        return found->second;
    }

    int
    ModuleState::
    gcVisit(
        visitproc visit,
        void * arg )
    {
        ::std::unique_lock< ::std::mutex > lock( this->_modulesMutex );
        for ( auto & [ name, module ] : this->_modules )
        {
            ::std::ignore = name;
            auto const result( module->gcVisit( visit, arg ) );
            if ( result != 0 )
            {
                return result;
            }
        }
        return 0;
    }
}
