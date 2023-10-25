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

#include "generated/io_oddsource_java_net_ifaddrs4j_MacAddress.h"

#include "cache.h"
#include "MacAddress.h"
#include "macros.h"

namespace
{
    jbyteArray
    data_to_byte_array(JNIEnv * env, OddSource::Interfaces::MacAddress const & address)
    {
        auto bytes(reinterpret_cast<jbyte const *>(static_cast<uint8_t const *>(address)));
        auto byte_array(env->NewByteArray(address.length()));
        IF_NULL_RETURN_NULL(byte_array)
        env->SetByteArrayRegion(byte_array, 0, address.length(), bytes);
        return byte_array;
    }
}

/**
 * @return io.oddsource.java.net.ifaddrs4j.MacAddress
 */
jobject
OddSource::ifaddrs4j::
convert_to_java(JNIEnv * env, OddSource::Interfaces::MacAddress const & address)
{
    jstring repr(env->NewStringUTF(::std::string(address).c_str()));
    IF_NULL_RETURN_NULL(repr)

    auto byte_array(data_to_byte_array(env, address));
    IF_NULL_RETURN_NULL(byte_array);

    return env->NewObject(JNICache::MacAddress, JNICache::MacAddress__init_, repr, byte_array);
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_MacAddress
 * Method:    get_data_from_repr
 * Signature: (Ljava/lang/String;)[B
 */
jbyteArray JNICALL Java_io_oddsource_java_net_ifaddrs4j_MacAddress_get_1data_1from_1repr(
    JNIEnv * env,
    jclass,
    jstring representation)
{
    using namespace OddSource::ifaddrs4j;

    if (representation == NULL || env->GetStringLength(representation) < 2)
    {
        env->ThrowNew(JNICache::IllegalArgumentException, "Representation must be a non-null, non-blank string");
        return NULL;
    }

    char const * repr_c_str(env->GetStringUTFChars(representation, NULL));
    IF_NULL_RETURN_NULL(repr_c_str)

    ::std::string repr_str(repr_c_str);
    env->ReleaseStringUTFChars(representation, repr_c_str);

    try
    {
        OddSource::Interfaces::MacAddress address(repr_str);
        return data_to_byte_array(env, address);
    }
    CATCH_STD_EXCEPTION_THROW_EXCEPTION_IF_NOT_THROWN(JNICache::IllegalArgumentException, return NULL)
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_MacAddress
 * Method:    get_repr_from_data
 * Signature: ([B)Ljava/lang/String;
 */
jstring JNICALL Java_io_oddsource_java_net_ifaddrs4j_MacAddress_get_1repr_1from_1data(
    JNIEnv * env,
    jclass,
    jbyteArray data)
{
    using namespace OddSource::ifaddrs4j;

    jsize length;
    if (data == NULL || (length = env->GetArrayLength(data)) < 1)
    {
        env->ThrowNew(JNICache::IllegalArgumentException, "Data must be a non-null, non-blank byte array");
        return NULL;
    }

    jbyte * bytes(env->GetByteArrayElements(data, NULL));
    IF_NULL_RETURN_NULL(bytes)

    try
    {
        OddSource::Interfaces::MacAddress address(reinterpret_cast<uint8_t const *>(bytes), length);
        env->ReleaseByteArrayElements(data, bytes, JNI_ABORT);
        jstring repr(env->NewStringUTF(::std::string(address).c_str()));
        return repr;
    }
    CATCH_STD_EXCEPTION_THROW_EXCEPTION_IF_NOT_THROWN(JNICache::IllegalArgumentException, return NULL)
}
