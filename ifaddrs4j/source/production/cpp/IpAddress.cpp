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

#include "IpAddress.h"
#include "macros.h"

// https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html
// https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/functions.html
// https://docs.oracle.com/en/java/javase/11/docs/specs/jni/design.html#accessing-fields-and-methods

/**
 * @return java.net.Inet4Address
 */
jobject
OddSource::ifaddrs4j::
convert_to_java(JNIEnv * env, OddSource::Interfaces::IPv4Address const & address)
{
    jclass const parent_cls = env->FindClass("java/net/InetAddress");
    IF_NULL_RETURN_NULL(parent_cls)

    // InetAddress getByAddress(byte[])
    auto const method(env->GetStaticMethodID(parent_cls, "getByAddress", "([B)Ljava/net/InetAddress;"));
    IF_NULL_RETURN_NULL(method)

    auto bytes(reinterpret_cast<jbyte const *>(static_cast<in_addr const *>(address)));
    /*jbyte jBytes[4];
    for(int i(0); i < 4; i++)
    {
        jBytes[i] = bytes[i];
    }*/
    auto byte_array(env->NewByteArray(4));
    IF_NULL_RETURN_NULL(byte_array)
    env->SetByteArrayRegion(byte_array, 0, 4, bytes);

    auto inet4Address(env->CallStaticObjectMethod(parent_cls, method, byte_array));
    IF_NULL_RETURN_NULL(inet4Address)

    jclass const cls = env->FindClass("java/net/Inet4Address");
    IF_NULL_RETURN_NULL(cls)
    if (!env->IsInstanceOf(inet4Address, cls))
    {
        env->ThrowNew(
            env->FindClass("java/io/IOException"),
            (::std::string("Unable to convert address ") + ::std::string(address) +
             " into a java.net.Inet4Address. Resulting object could not be cast.").c_str());
        return NULL;
    }

    return inet4Address;
}

/**
 * @return java.net.Inet6Address
 */
jobject
OddSource::ifaddrs4j::
convert_to_java(JNIEnv * env, OddSource::Interfaces::IPv6Address const & address)
{
    jclass const parent_cls = env->FindClass("java/net/InetAddress");
    IF_NULL_RETURN_NULL(parent_cls)

    jclass const cls(env->FindClass("java/net/Inet6Address"));
    IF_NULL_RETURN_NULL(cls)

    jstring repr(env->NewStringUTF(::std::string(address).c_str()));
    IF_NULL_RETURN_NULL(repr)

    auto bytes(reinterpret_cast<jbyte const *>(static_cast<in6_addr const *>(address)));
    auto byte_array(env->NewByteArray(16));
    IF_NULL_RETURN_NULL(byte_array)
    env->SetByteArrayRegion(byte_array, 0, 16, bytes);

    if (address.scope_id() && *address.scope_id() > 0)
    {
        // Inet6Address getByAddress(String, byte[], int)
        auto const method(env->GetStaticMethodID(cls,
                                                "getByAddress",
                                                "(Ljava/lang/String;[BI)Ljava/net/Inet6Address;"));
        IF_NULL_RETURN_NULL(method)

        jint scope_id(*address.scope_id());

        return env->CallStaticObjectMethod(cls, method, repr, byte_array, scope_id);
    }
    else
    {
        // InetAddress getByAddress(byte[])
        auto const method(env->GetStaticMethodID(parent_cls, "getByAddress", "([B)Ljava/net/InetAddress;"));
        IF_NULL_RETURN_NULL(method)

        auto inet6Address(env->CallStaticObjectMethod(parent_cls, method, byte_array));
        IF_NULL_RETURN_NULL(inet6Address)

        if (!env->IsInstanceOf(inet6Address, cls))
        {
            env->ThrowNew(
                env->FindClass("java/io/IOException"),
                (::std::string("Unable to convert address ") + ::std::string(address) +
                 " into a java.net.Inet6Address. Resulting object could not be cast.").c_str());
            return NULL;
        }

        return inet6Address;
    }
}
