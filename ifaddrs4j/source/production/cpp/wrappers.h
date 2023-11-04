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

#include <jni.h>
#include <optional>
#include <string>

namespace OddSource::ifaddrs4j
{
    class ArrayList
    {
    public:
        ArrayList(JNIEnv *);

        ArrayList(JNIEnv *, jint);

        [[nodiscard]]
        bool is_valid();

        [[nodiscard]]
        bool add(jobject);

        [[nodiscard]]
        jobject unwrap();

    private:
        ArrayList(JNIEnv *, bool);

        static ::std::string const class_name;

        JNIEnv * _env;
        jclass _class;
        jmethodID _add;
        jobject _wrapped;
    };

    class Function
    {
    public:
        Function(JNIEnv *, jobject);

        [[nodiscard]]
        bool is_valid();

        [[nodiscard]]
        jobject operator()(jobject);

    private:
        static ::std::string const class_name;

        JNIEnv * _env;
        jobject _wrapped;
        jclass _class;
        jmethodID _apply;
    };

    class BooleanUnboxer
    {
    public:
        BooleanUnboxer(JNIEnv *);

        [[nodiscard]]
        bool is_valid();

        [[nodiscard]]
        bool operator()(jobject);

    private:
        static ::std::string const class_name;

        JNIEnv * _env;
        jclass _class;
        jmethodID _booleanValue;
    };

    class Boxers
    {
    public:
        static jobject Short(JNIEnv *, ::std::optional<uint8_t const> const &);

        static jobject Integer(JNIEnv *, ::std::optional<uint32_t const> const &);

        static jobject Long(JNIEnv *, ::std::optional<uint64_t const> const &);

    private:
        Boxers() = default;
    };
}
