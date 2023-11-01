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

namespace OddSource::ifaddrs4j
{
    class JNICache
    {
    public:
        static jclass InetAddressHelper;
        static jmethodID InetAddressHelper_getIPv4Address;
        static jmethodID InetAddressHelper_getIPv6Address;

        static jclass MacAddress;
        static jmethodID MacAddress__init_;

        static jclass InterfaceIPAddress;
        static jmethodID InterfaceIPAddress__init_;

        static jclass Interface;
        static jmethodID Interface__init_;

        static jclass Boolean;
        static jmethodID Boolean_booleanValue;

        static jclass Short;
        static jmethodID Short_valueOf;

        static jclass Integer;
        static jmethodID Integer_valueOf;

        static jclass Long;
        static jmethodID Long_valueOf;

        static jclass ArrayList;
        static jmethodID ArrayList__init_;
        static jmethodID ArrayList__init__int;
        static jmethodID ArrayList_add;

        static jclass Function;
        static jmethodID Function_apply;

        static jclass IllegalArgumentException;
        static jclass IllegalStateException;
        static jclass EnumConstantNotPresentException;
        static jclass RuntimeException;

        static bool ensure_our_classes_loaded(JNIEnv *);

    private:
        JNICache() = delete;
    };
}

#define ENSURE_OUR_CLASSES_LOADED(jnienv, ret) if (!OddSource::ifaddrs4j::JNICache::ensure_our_classes_loaded(jnienv)) \
        { \
            return ret; \
        }

jint JNI_OnLoad(JavaVM *, void *);

void JNI_OnUnload(JavaVM *, void *);
