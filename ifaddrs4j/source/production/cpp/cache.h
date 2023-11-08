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

#include "common.h"

#include <memory>
#include <mutex>
#include <unordered_map>

jint JNI_OnLoad(JavaVM *, void *);

void JNI_OnUnload(JavaVM *, void *);

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

        static jclass c(JNIEnv *, ::std::string);

        static jmethodID m(JNIEnv *, ::std::string, ::std::string);

        static jmethodID m(JNIEnv *, jclass, ::std::string, ::std::string);

        static jmethodID sm(JNIEnv *, jclass, ::std::string, ::std::string);

    private:
        friend jint (::JNI_OnLoad)(JavaVM *, void *);
        friend void (::JNI_OnUnload)(JavaVM *, void *);

        ClassMethodCache();

        static void create_instance(JNIEnv *);

        static void destroy_instance(JNIEnv *);

        static bool ensure_singleton(JNIEnv *);

        static void IllegalStateException(JNIEnv *, char const *);

        static ::std::mutex _singleton_mutex;
        static ::std::unique_ptr<ClassMethodCache> _singleton;
        static ::std::unordered_map<::std::string, ClassSearchPath> const _class_name_to_canon;
        static ::std::unordered_map<::std::string, MethodSignature> const _method_name_to_signature;
        static ::std::unordered_map<::std::string, MethodSignature> const _static_method_name_to_signature;

        jclass get_class(JNIEnv *, ::std::string);

        jmethodID get_method(JNIEnv *, ::std::string, ::std::string);

        jmethodID get_method(JNIEnv *, jclass, ::std::string, ::std::string);

        jmethodID get_static_method(JNIEnv *, jclass, ::std::string, ::std::string);

        jmethodID get_any_method(
            JNIEnv *,
            jclass,
            ::std::string,
            ::std::string,
            ::std::unordered_map<::std::string, MethodSignature> const &,
            ::std::unordered_map<::std::string, jmethodID> &,
            bool is_static = false);

        ::std::recursive_mutex _mutex;
        ::std::unordered_map<::std::string, jclass> _class_cache;
        ::std::unordered_map<::std::string, jmethodID> _method_cache;
        ::std::unordered_map<::std::string, jmethodID> _static_method_cache;
    };

    typedef ClassMethodCache JCache;
}
