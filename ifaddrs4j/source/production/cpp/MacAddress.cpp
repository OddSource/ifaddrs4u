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

#include "MacAddress.h"
#include "macros.h"

/**
 * @return io.oddsource.java.net.ifaddrs4j.MacAddress
 */
jobject
OddSource::ifaddrs4j::
convert_to_java(JNIEnv * env, OddSource::Interfaces::MacAddress const & address)
{
    jclass cls = env->FindClass("io/oddsource/java/net/ifaddrs4j/MacAddress");
    IF_NULL_RETURN_NULL(cls)

    // MacAddress(String, byte[])
    auto constructor(env->GetMethodID(cls, "<init>", "(Ljava/lang/String;[B)V"));
    IF_NULL_RETURN_NULL(constructor)

    jstring repr(env->NewStringUTF(::std::string(address).c_str()));
    IF_NULL_RETURN_NULL(repr)

    auto bytes(reinterpret_cast<jbyte const *>(static_cast<uint8_t const *>(address)));
    auto byte_array(env->NewByteArray(address.length()));
    IF_NULL_RETURN_NULL(byte_array)
    env->SetByteArrayRegion(byte_array, 0, address.length(), bytes);

    return env->NewObject(cls, constructor, repr, byte_array);
}
