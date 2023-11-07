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

    auto JMacAddress(JCache::c(env, "MacAddress"));
    IF_NULL_RETURN_NULL(JMacAddress)
    auto constructor(JCache::m(env, JMacAddress, "MacAddress", "MacAddress(...)"));
    IF_NULL_RETURN_NULL(constructor)
    return env->NewObject(JMacAddress, constructor, repr, byte_array);
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_MacAddress
 * Method:    getDataFromRepr
 * Signature: (Ljava/lang/String;)[B
 */
jbyteArray JNICALL Java_io_oddsource_java_net_ifaddrs4j_MacAddress_getDataFromRepr(
    JNIEnv * env,
    jclass,
    jstring representation)
{
    using namespace OddSource::ifaddrs4j;

    if (representation == NULL || env->GetStringLength(representation) < 2)
    {
        auto IllegalArgumentException(JCache::c(env, "IllegalArgumentException"));
        IF_NULL_RETURN_NULL(IllegalArgumentException)
        env->ThrowNew(IllegalArgumentException, "Representation must be a non-null, non-blank string");
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
    CATCH_STD_EXCEPTION_THROW_EXCEPTION_IF_NOT_THROWN("IllegalArgumentException", return NULL)
}

namespace
{
    class ByteArrayElementsReleaser
    {
    public:
        ByteArrayElementsReleaser(JNIEnv * env, jbyteArray data, jbyte * bytes)
            : _env(env), _data(data), _bytes(bytes)
        {
        }

        ~ByteArrayElementsReleaser()
        {
            this->_env->ReleaseByteArrayElements(this->_data, this->_bytes, JNI_ABORT);
        }

    private:
        JNIEnv * _env;
        jbyteArray _data;
        jbyte * _bytes;
    };
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_MacAddress
 * Method:    getReprFromData
 * Signature: ([B)Ljava/lang/String;
 */
jstring JNICALL Java_io_oddsource_java_net_ifaddrs4j_MacAddress_getReprFromData(
    JNIEnv * env,
    jclass,
    jbyteArray data)
{
    using namespace OddSource::ifaddrs4j;

    jsize length;
    if (data == NULL || (length = env->GetArrayLength(data)) < 1)
    {
        auto IllegalArgumentException(JCache::c(env, "IllegalArgumentException"));
        IF_NULL_RETURN_NULL(IllegalArgumentException)
        env->ThrowNew(IllegalArgumentException, "Data must be a non-null, non-blank byte array");
        return NULL;
    }

    jbyte * bytes(env->GetByteArrayElements(data, NULL));
    IF_NULL_RETURN_NULL(bytes)

    // will automatically release the byte array elements when the function returns, however it returns
    ByteArrayElementsReleaser releaser(env, data, bytes);

    try
    {
#ifdef IS_WINDOWS
// no loss of data as indicated by MSVC
#pragma warning( push )
#pragma warning( disable : 4244)
#endif /* IS_WINDOWS */
        OddSource::Interfaces::MacAddress address(reinterpret_cast<uint8_t const *>(bytes), length);
#ifdef IS_WINDOWS
#pragma warning( pop )
#endif /* IS_WINDOWS */
        jstring repr(env->NewStringUTF(::std::string(address).c_str()));
        return repr;
    }
    CATCH_STD_EXCEPTION_THROW_EXCEPTION_IF_NOT_THROWN("IllegalArgumentException", return NULL)
}
