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

#include "common.h"

#include <memory>
#include <mutex>
#include <unordered_map>

jint
JNI_OnLoad(
    JavaVM *,
    void * );

void
JNI_OnUnload(
    JavaVM *,
    void * );

namespace OddSource::ifaddrs4j
{
    struct ClassSearchPath
    {
        char const * path;
    };

    struct MethodSignature
    {
        char const * name;
        char const * signature;
    };

    class ClassMethodCache
    {
    public:
        ~ClassMethodCache();

        ClassMethodCache(
            ClassMethodCache const & ) = delete;

        ClassMethodCache(
            ClassMethodCache && ) = delete;

        ClassMethodCache &
        operator=(
            ClassMethodCache const & ) = delete;

        ClassMethodCache &
        operator=(
            ClassMethodCache && ) = delete;

        static
        jclass
        c(
            JNIEnv * pEnv,
            ::std::string className );

        static
        jmethodID
        m(
            JNIEnv * pEnv,
            ::std::string className,
            ::std::string methodName );

        static
        jmethodID
        m(
            JNIEnv * pEnv,
            jclass pClass,
            ::std::string className,
            ::std::string methodName );

        static
        jmethodID
        sm(
            JNIEnv * pEnv,
            jclass pClass,
            ::std::string className,
            ::std::string methodName );

    private:
        ClassMethodCache();

        friend
        jint
        (::JNI_OnLoad)(
            JavaVM *,
            void * );

        friend
        void
        (::JNI_OnUnload)(
            JavaVM *,
            void * );

        static
        void
        create_instance(
            JNIEnv * pEnv );

        static
        void
        destroy_instance(
            JNIEnv * pEnv );

        static
        bool
        ensure_singleton(
            JNIEnv * pEnv );

        static
        void
        IllegalStateException(
            JNIEnv * pEnv,
            char const * message );

        static ::std::mutex _singleton_mutex;
        static ::std::unique_ptr< ClassMethodCache > _singleton;
        static ::std::unordered_map< ::std::string, ClassSearchPath > const _class_name_to_canon;
        static ::std::unordered_map< ::std::string, MethodSignature > const _method_name_to_signature;
        static ::std::unordered_map< ::std::string, MethodSignature > const _static_method_name_to_signature;

        jclass
        get_class(
            JNIEnv * pEnv,
            ::std::string className );

        jmethodID
        get_method(
            JNIEnv * pEnv,
            ::std::string className,
            ::std::string methodName );

        jmethodID
        get_method(
            JNIEnv * pEnv,
            jclass pClass,
            ::std::string className,
            ::std::string methodName );

        jmethodID
        get_static_method(
            JNIEnv * pEnv,
            jclass pClass,
            ::std::string className,
            ::std::string methodName );

        jmethodID
        get_any_method(
            JNIEnv * pEnv,
            jclass pClass,
            ::std::string className,
            ::std::string methodName,
            ::std::unordered_map< ::std::string, MethodSignature > const & signatureMap,
            ::std::unordered_map< ::std::string, jmethodID > & methodCache,
            bool isStatic = false );

        ::std::recursive_mutex _mutex;
        ::std::unordered_map< ::std::string, jclass > _class_cache;
        ::std::unordered_map< ::std::string, jmethodID > _method_cache;
        ::std::unordered_map< ::std::string, jmethodID > _static_method_cache;
    };

    typedef ClassMethodCache JCache;
}
