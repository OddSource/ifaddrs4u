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

import static org.junit.Assert.*;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Locale;
import java.util.UUID;

import org.junit.Test;

public class TestSamples
{
    @Test
    public void testGetIPv4Loopback()
    {
        final var address = Samples.getIPv4Loopback();
        assertNotNull(address);
        assertEquals("127.0.0.1", address.getHostAddress());
    }

    @Test
    public void testGetIPv6Loopback() throws UnknownHostException
    {
        assertEquals("0:0:0:0:0:0:0:1", InetAddress.getByAddress(
            new byte[] {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}
        ).getHostAddress());

        final var address = Samples.getIPv6Loopback();
        assertNotNull(address);
        assertEquals("0:0:0:0:0:0:0:1", address.getHostAddress());
    }

    @Test
    public void testMacAddress()
    {
        final var address = Samples.getMacAddress();
        assertNotNull(address);
        assertEquals("ac:de:48:00:11:22", address.toString());
    }

    @Test
    public void testGetInterfaceIPv4Address()
    {
        final var address = Samples.getInterfaceIPv4Address();
        assertNotNull(address);
        assertEquals("192.168.0.42", address.getAddress().getHostAddress());
        assertEquals(Short.valueOf((short) 24), address.getPrefixLength());
        assertNotNull(address.getBroadcastAddress());
        assertEquals("192.168.0.254", address.getBroadcastAddress().getHostAddress());
        assertNull(address.getPointToPointDestination());
        assertEquals("192.168.0.42/24 broadcast 192.168.0.254", address.toString());
    }

    @Test
    public void testGetInterfaceIPv6Address()
    {
        final var address = Samples.getInterfaceIPv6Address();
        assertNotNull(address);
        assertEquals("2001:470:2ccb:a61b:e:acf8:6736:d81e", address.getAddress().getHostAddress());
        assertEquals(Short.valueOf((short) 64), address.getPrefixLength());
        assertNull(address.getBroadcastAddress());
        assertNull(address.getPointToPointDestination());
        assertTrue(address.isFlagEnabled(InterfaceIPAddressFlag.AutoConfigured));
        assertTrue(address.isFlagEnabled(InterfaceIPAddressFlag.Secured));
        assertEquals("2001:470:2ccb:a61b:e:acf8:6736:d81e/64 autoconf secured", address.toString());
    }

    @Test
    public void testGetInterface()
    {
        final var anInterface = Samples.getInterface();
        assertNotNull(anInterface);
        assertEquals(3, anInterface.getIndex());
        assertEquals("en0", anInterface.getName());

        if (System.getProperty("os.name").toLowerCase(Locale.US).contains("win"))
        {
            assertEquals(UUID.fromString("24af9519-2a42-4f62-99fa-1ed3147ad90a"), anInterface.getWindowsUuid());
        }

        assertEquals("ac:de:48:00:11:22", anInterface.getMacAddress().toString());
        final var iterV4 = anInterface.getIpv4Addresses();
        assertTrue(iterV4.hasNext());
        assertEquals("192.168.0.42", iterV4.next().getAddress().getHostAddress());
        assertFalse(iterV4.hasNext());
        final var iterV6 = anInterface.getIpv6Addresses();
        assertTrue(iterV6.hasNext());
        assertEquals("2001:470:2ccb:a61b:e:acf8:6736:d81f", iterV6.next().getAddress().getHostAddress());
        assertFalse(iterV6.hasNext());
    }
}
