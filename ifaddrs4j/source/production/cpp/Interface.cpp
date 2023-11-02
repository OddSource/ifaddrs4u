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

#include "generated/io_oddsource_java_net_ifaddrs4j_EnumConstantsHelper.h"

#include <ifaddrs4cpp/Interface.h>

#include "cache.h"
#include "common.h"
#include "Interface.h"
#include "IpAddress.h"
#include "MacAddress.h"
#include "macros.h"

#define UNSUPPORTED_FLAG_NAME "UNSUPPORTED_FLAG"
#define UNSUPPORTED_FLAG 2147483645

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_InterfaceFlag
 * Method:    getConstant
 * Signature: (Ljava/lang/String;)I
 */
jint JNICALL Java_io_oddsource_java_net_ifaddrs4j_EnumConstantsHelper_getInterfaceFlagConstant(
    JNIEnv * env,
    jclass,
    jstring enum_name)
{
    using namespace OddSource::Interfaces;

    char const * enum_name_c_str(env->GetStringUTFChars(enum_name, 0));
    IF_NULL_RETURN_INT(enum_name_c_str)

    ::std::string enum_name_str(enum_name_c_str);
    env->ReleaseStringUTFChars(enum_name, enum_name_c_str);

    if (enum_name_str == UNSUPPORTED_FLAG_NAME)
    {
        return UNSUPPORTED_FLAG;
    }

    auto found = InterfaceFlag_Values.find(enum_name_str);
    if (found == InterfaceFlag_Values.end())
    {
        return UNSUPPORTED_FLAG;
    }

    return found->second;
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_InterfaceIPAddressFlag
 * Method:    getConstant
 * Signature: (Ljava/lang/String;)I
 */
jint JNICALL Java_io_oddsource_java_net_ifaddrs4j_EnumConstantsHelper_getInterfaceIPAddressFlagConstant(
    JNIEnv * env,
    jclass,
    jstring enum_name)
{
    using namespace OddSource::Interfaces;
    using namespace OddSource::ifaddrs4j;

    char const * enum_name_c_str(env->GetStringUTFChars(enum_name, 0));
    IF_NULL_RETURN_INT(enum_name_c_str)

    ::std::string enum_name_str(enum_name_c_str);
    env->ReleaseStringUTFChars(enum_name, enum_name_c_str);

    auto found = InterfaceIPAddressFlag_Values.find(enum_name_str);
    if (found == InterfaceIPAddressFlag_Values.end())
    {
        env->ThrowNew(
            JNICache::EnumConstantNotPresentException,
            ("Enum constant "s + enum_name_str +
             " did not match any constants in the underlying C++ API, "s +
             "likely mismatch between Java code and C++ extension"s).c_str());
        return -1;
    }

    return found->second;
}

/**
 * @return io.oddsource.java.net.ifaddrs4j.MacAddress
 */
template<class IPAddressT>
jobject
OddSource::ifaddrs4j::
convert_to_java(JNIEnv * env, OddSource::Interfaces::InterfaceIPAddress<IPAddressT> const & interface_address)
{
    ENSURE_OUR_CLASSES_LOADED(env, NULL)

    auto address(convert_to_java(env, interface_address.address()));
    IF_NULL_RETURN_NULL(address)

    jint flags(interface_address.flags());

    jobject prefix_length = NULL;
    if (interface_address.prefix_length())
    {
        prefix_length = env->CallStaticObjectMethod(
            JNICache::Short,
            JNICache::Short_valueOf,
            *interface_address.prefix_length());
        IF_NULL_RETURN_NULL(prefix_length);
    }

    jobject broadcast = NULL;
    if (interface_address.broadcast_address())
    {
        broadcast = convert_to_java(env, *interface_address.broadcast_address());
        IF_NULL_RETURN_NULL(broadcast)
    }

    jobject point_to_point = NULL;
    if (interface_address.point_to_point_destination())
    {
        point_to_point = convert_to_java(env, *interface_address.point_to_point_destination());
        IF_NULL_RETURN_NULL(point_to_point)
    }

    return env->NewObject(
        JNICache::InterfaceIPAddress,
        JNICache::InterfaceIPAddress__init_,
        address, flags, prefix_length, broadcast, point_to_point);
}

/**
 * @return io.oddsource.java.net.ifaddrs4j.MacAddress
 */
jobject
OddSource::ifaddrs4j::
convert_to_java(JNIEnv * env, OddSource::Interfaces::Interface const & iface)
{
    ENSURE_OUR_CLASSES_LOADED(env, NULL)

    using namespace OddSource::Interfaces;

    jint index(iface.index());

    jstring name(env->NewStringUTF(iface.name().c_str()));
    IF_NULL_RETURN_NULL(name)

#ifdef IS_WINDOWS
    jstring windows_uuid(env->NewStringUTF(iface.windows_uuid()));
    IF_NULL_RETURN_NULL(windows_uuid)
#endif /* IS_WINDOWS */

    jint flags(iface.flags());

    jobject mtu = NULL;
    if (iface.mtu())
    {
        mtu = env->CallStaticObjectMethod(JNICache::Long, JNICache::Long_valueOf, *iface.mtu());
        IF_NULL_RETURN_NULL(mtu);
    }

    jobject mac_address = NULL;
    if (iface.mac_address())
    {
        mac_address = convert_to_java(env, *iface.mac_address());
        IF_NULL_RETURN_NULL(mac_address)
    }

    auto ipv4_addresses(env->NewObject(
        JNICache::ArrayList,
        JNICache::ArrayList__init__int,
        (jint) iface.ipv4_addresses().size()));
    IF_NULL_RETURN_NULL(ipv4_addresses);
    auto ipv6_addresses(env->NewObject(
        JNICache::ArrayList,
        JNICache::ArrayList__init__int,
        (jint) iface.ipv6_addresses().size()));
    IF_NULL_RETURN_NULL(ipv6_addresses);

    auto v4(iface.ipv4_addresses());
    for (auto const & address : v4)
    {
        auto converted(convert_to_java(env, address));
        IF_NULL_RETURN_NULL(converted);
        if (!env->CallBooleanMethod(ipv4_addresses, JNICache::ArrayList_add, converted))
        {
            return NULL;
        }
    }

    auto v6(iface.ipv6_addresses());
    for (auto const & address : v6)
    {
        auto converted(convert_to_java(env, address));
        IF_NULL_RETURN_NULL(converted);
        if (!env->CallBooleanMethod(ipv6_addresses, JNICache::ArrayList_add, converted))
        {
            return NULL;
        }
    }

    return env->NewObject(
        JNICache::Interface,
        JNICache::Interface__init_,
        index, name,
#ifdef IS_WINDOWS
        windows_uuid,
#endif /* IS_WINDOWS */
        flags, mtu, mac_address, ipv4_addresses, ipv6_addresses);
}
