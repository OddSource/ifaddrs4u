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

package io.oddsource.java.net.ifaddrs4j;

import java.net.Inet4Address;
import java.net.Inet6Address;

/**
 * A class for obtaining samples of {@code OddSource::Interfaces::*}} objects converted to Java.
 *
 * @since 1.0.0
 */
public final class Samples
{
    static
    {
        if(!Loader.LOADED)
        {
            // this will never actually happen, but we need to ensure that Loader initializes
            throw new RuntimeException("Not possible!");
        }
    }

    private Samples()
    {
        throw new UnsupportedOperationException();
    }

    /**
     * A sample IPv4 address.
     *
     * @return 127.0.0.1.
     */
    public static native Inet4Address getIPv4Loopback();

    /**
     * A sample IPv6 address.
     *
     * @return ::1
     */
    public static native Inet6Address getIPv6Loopback();

    /**
     * A sample MAC address.
     *
     * @return ac:de:48:00:11:22
     */
    public static native MacAddress getMacAddress();

    /**
     * A sample IPv4 address with broadcast.
     *
     * @return 192.168.0.42/24 broadcast 192.168.0.254
     */
    public static native InterfaceIPAddress<Inet4Address> getInterfaceIPv4Address();

    /**
     * A sample IPv6 address.
     *
     * @return 2001:470:2ccb:a61b:e:acf8:6736:d81e/64 autoconf secured
     */
    public static native InterfaceIPAddress<Inet6Address> getInterfaceIPv6Address();

    /**
     * A sample scoped IPv6 address.
     *
     * @return fe80::aede:48ff:fe00:1122%en5/56 secured scopeid en5
     */
    public static native InterfaceIPAddress<Inet6Address> getInterfaceScopedIPv6Address();

    /**
     * A sample interface.
     *
     * @return TBD
     */
    public static native Interface getInterface();
}
