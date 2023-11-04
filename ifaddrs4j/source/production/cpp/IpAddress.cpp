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

#include "cache.h"
#include "IpAddress.h"
#include "macros.h"
#include "wrappers.h"

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
    auto bytes(reinterpret_cast<jbyte const *>(static_cast<in_addr const *>(address)));
    auto byte_array(env->NewByteArray(4));
    IF_NULL_RETURN_NULL(byte_array)
    env->SetByteArrayRegion(byte_array, 0, 4, bytes);

    auto InetAddressHelper(JCache::c(env, "InetAddressHelper"));
    IF_NULL_RETURN_NULL(InetAddressHelper)
    auto getIPv4Address(JCache::sm(env, InetAddressHelper, "InetAddressHelper", "getIPv4Address(byte[])"));
    IF_NULL_RETURN_NULL(getIPv4Address)
    return env->CallStaticObjectMethod(InetAddressHelper, getIPv4Address, byte_array);
}

/**
 * @return java.net.Inet6Address
 */
jobject
OddSource::ifaddrs4j::
convert_to_java(JNIEnv * env, OddSource::Interfaces::IPv6Address const & address)
{
    auto bytes(reinterpret_cast<jbyte const *>(static_cast<in6_addr const *>(address)));
    auto byte_array(env->NewByteArray(16));
    IF_NULL_RETURN_NULL(byte_array)
    env->SetByteArrayRegion(byte_array, 0, 16, bytes);

    jobject scope_id(Boxers::Integer(env, address.scope_id()));
    if (env->ExceptionOccurred() != NULL)
    {
        return NULL;
    }

    auto InetAddressHelper(JCache::c(env, "InetAddressHelper"));
    IF_NULL_RETURN_NULL(InetAddressHelper)
    auto getIPv6Address(JCache::sm(env, InetAddressHelper, "InetAddressHelper", "getIPv6Address(byte[], Integer)"));
    IF_NULL_RETURN_NULL(getIPv6Address)
    return env->CallStaticObjectMethod(InetAddressHelper, getIPv6Address, byte_array, scope_id);
}
