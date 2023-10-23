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

#include "generated/io_oddsource_java_net_ifaddrs4j_Samples.h"

#include <ifaddrs4cpp/IpAddress.h>
#include <ifaddrs4cpp/MacAddress.h>
#include <ifaddrs4cpp/Interface.h>

#include "Interface.h"
#include "IpAddress.h"
#include "MacAddress.h"
#include "macros.h"

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_Samples
 * Method:    getIPv4Loopback
 * Signature: ()Ljava/net/Inet4Address;
 */
jobject JNICALL Java_io_oddsource_java_net_ifaddrs4j_Samples_getIPv4Loopback(
    JNIEnv * env,
    jclass)
{
    static OddSource::Interfaces::IPv4Address const LO_V4("127.0.0.1");
    return OddSource::ifaddrs4j::convert_to_java(env, LO_V4);
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_Samples
 * Method:    getIPv6Loopback
 * Signature: ()Ljava/net/Inet6Address;
 */
jobject JNICALL Java_io_oddsource_java_net_ifaddrs4j_Samples_getIPv6Loopback(
    JNIEnv * env,
    jclass)
{
    static OddSource::Interfaces::IPv6Address const LO_V6("::1");
    return OddSource::ifaddrs4j::convert_to_java(env, LO_V6);
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_Samples
 * Method:    getMacAddress
 * Signature: ()Lio/oddsource/java/net/ifaddrs4j/MacAddress;
 */
jobject JNICALL Java_io_oddsource_java_net_ifaddrs4j_Samples_getMacAddress(
    JNIEnv * env,
    jclass)
{
    static OddSource::Interfaces::MacAddress const MAC("ac:de:48:00:11:22");
    return OddSource::ifaddrs4j::convert_to_java(env, MAC);
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_Samples
 * Method:    getInterfaceIPv4Address
 * Signature: ()Lio/oddsource/java/net/ifaddrs4j/InterfaceIPAddress;
 */
jobject JNICALL Java_io_oddsource_java_net_ifaddrs4j_Samples_getInterfaceIPv4Address(
    JNIEnv * env,
    jclass)
{
    static OddSource::Interfaces::InterfaceIPv4Address const IPv4(
        OddSource::Interfaces::IPv4Address("192.168.0.42"),
        0,
        24u,
        OddSource::Interfaces::IPv4Address("192.168.0.254"));
    return OddSource::ifaddrs4j::convert_to_java(env, IPv4);
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_Samples
 * Method:    getInterfaceIPv6Address
 * Signature: ()Lio/oddsource/java/net/ifaddrs4j/InterfaceIPAddress;
 */
jobject JNICALL Java_io_oddsource_java_net_ifaddrs4j_Samples_getInterfaceIPv6Address(
    JNIEnv * env,
    jclass)
{
    static OddSource::Interfaces::InterfaceIPv6Address const IPv6(
        OddSource::Interfaces::IPv6Address("2001:470:2ccb:a61b:e:acf8:6736:d81e"),
        OddSource::Interfaces::AutoConfigured | OddSource::Interfaces::Secured,
        56u);
    return OddSource::ifaddrs4j::convert_to_java(env, IPv6);
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_Samples
 * Method:    getInterfaceScopedIPv6Address
 * Signature: ()Lio/oddsource/java/net/ifaddrs4j/InterfaceIPAddress;
 */
jobject JNICALL Java_io_oddsource_java_net_ifaddrs4j_Samples_getInterfaceScopedIPv6Address(
    JNIEnv * env,
    jclass
)
{
    using namespace OddSource::Interfaces;
    static InterfaceIPv6Address const Scoped_IPv6(
        IPv6Address(static_cast<in6_addr const *>(IPv6Address("fe80::aede:48ff:fe00:1122")), v6Scope {6, "en5"}),
        Secured,
        64u);
    return OddSource::ifaddrs4j::convert_to_java(env, Scoped_IPv6);
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_Samples
 * Method:    getInterface
 * Signature: ()Lio/oddsource/java/net/ifaddrs4j/Interface;
 */
jobject JNICALL Java_io_oddsource_java_net_ifaddrs4j_Samples_getInterface(
    JNIEnv * env,
    jclass)
{
    return OddSource::ifaddrs4j::convert_to_java(env, OddSource::Interfaces::Interface::SAMPLE_INTERFACE);
}
