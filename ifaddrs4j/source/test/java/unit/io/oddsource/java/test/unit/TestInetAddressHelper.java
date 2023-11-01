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

package io.oddsource.java.test.unit;

import static org.junit.Assert.*;

import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.UnknownHostException;

import org.junit.Test;

import io.oddsource.java.net.ifaddrs4j.InetAddressHelper;

public class TestInetAddressHelper
{
    public TestInetAddressHelper()
    {
    }

    private static byte[] bytes(final int... args)
    {
        final byte[] value = new byte[args.length];
        for(int i = 0; i < args.length; i++)
        {
            value[i] = (byte) args[i];
        }
        return value;
    }

    @Test
    public void testIPv4Address1() throws UnknownHostException
    {
        final Inet4Address address = InetAddressHelper.getIPv4Address(bytes(172, 16, 52, 4));
        assertNotNull(address);
        assertArrayEquals(bytes(172, 16, 52, 4), address.getAddress());
        assertEquals("172.16.52.4", address.getHostAddress());
        assertEquals("172.16.52.4", InetAddressHelper.toString(address));
    }

    @Test
    public void testIPv4Address2() throws UnknownHostException
    {
        final Inet4Address address = InetAddressHelper.getIPv4Address(bytes(127, 0, 0, 1));
        assertNotNull(address);
        assertArrayEquals(bytes(127, 0, 0, 1), address.getAddress());
        assertEquals("127.0.0.1", address.getHostAddress());
        assertEquals("127.0.0.1", InetAddressHelper.toString(address));
    }

    @Test
    public void testIPv4Address3() throws UnknownHostException
    {
        final Inet4Address address = InetAddressHelper.getIPv4Address(bytes(239, 17, 0, 134));
        assertNotNull(address);
        assertArrayEquals(bytes(239, 17, 0, 134), address.getAddress());
        assertEquals("239.17.0.134", address.getHostAddress());
        assertEquals("239.17.0.134", InetAddressHelper.toString(address));
    }

    @Test
    public void testIPv6AddressUncompressed() throws UnknownHostException
    {
        final Inet6Address address = InetAddressHelper.getIPv6Address(
            bytes(0x20, 0x01, 0x4, 0x71, 0xbc, 0xc2, 0xab, 0x61, 0x1d, 0xec, 0x53, 0x96, 0x52, 0x7a, 0xcb, 0xe0),
            null
        );
        assertNotNull(address);
        assertArrayEquals(
            bytes(0x20, 0x01, 0x4, 0x71, 0xbc, 0xc2, 0xab, 0x61, 0x1d, 0xec, 0x53, 0x96, 0x52, 0x7a, 0xcb, 0xe0),
            address.getAddress()
        );
        assertEquals("2001:471:bcc2:ab61:1dec:5396:527a:cbe0", address.getHostAddress());
        assertEquals("2001:471:bcc2:ab61:1dec:5396:527a:cbe0", InetAddressHelper.toString(address));
    }

    @Test
    public void testIPv6AddressCompressedAtBeginning() throws UnknownHostException
    {
        final Inet6Address address = InetAddressHelper.getIPv6Address(
            bytes(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1),
            null
        );
        assertNotNull(address);
        assertArrayEquals(
            bytes(0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1),
            address.getAddress()
        );
        assertEquals("0:0:0:0:0:0:0:1", address.getHostAddress());
        assertEquals("::1", InetAddressHelper.toString(address));
    }

    @Test
    public void testIPv6AddressCompressedAtEnd() throws UnknownHostException
    {
        final Inet6Address address = InetAddressHelper.getIPv6Address(
            bytes(0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0),
            null
        );
        assertNotNull(address);
        assertArrayEquals(
            bytes(0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0),
            address.getAddress()
        );
        assertEquals("1:0:0:0:0:0:0:0", address.getHostAddress());
        assertEquals("1::", InetAddressHelper.toString(address));
    }

    @Test
    public void testIPv6AddressCompressedMiddleNear() throws UnknownHostException
    {
        final Inet6Address address = InetAddressHelper.getIPv6Address(
            bytes(0x20, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1d, 0xec, 0x53, 0x96, 0x52, 0x7a, 0xcb, 0xe0),
            null
        );
        assertNotNull(address);
        assertArrayEquals(
            bytes(0x20, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1d, 0xec, 0x53, 0x96, 0x52, 0x7a, 0xcb, 0xe0),
            address.getAddress()
        );
        assertEquals("2001:0:0:0:1dec:5396:527a:cbe0", address.getHostAddress());
        assertEquals("2001::1dec:5396:527a:cbe0", InetAddressHelper.toString(address));
    }

    @Test
    public void testIPv6AddressCompressedMiddleFar() throws UnknownHostException
    {
        final Inet6Address address = InetAddressHelper.getIPv6Address(
            bytes(0x20, 0x01, 0x4, 0x71, 0xbc, 0xc2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xcb, 0xe0),
            null
        );
        assertNotNull(address);
        assertArrayEquals(
            bytes(0x20, 0x01, 0x4, 0x71, 0xbc, 0xc2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xcb, 0xe0),
            address.getAddress()
        );
        assertEquals("2001:471:bcc2:0:0:0:0:cbe0", address.getHostAddress());
        assertEquals("2001:471:bcc2::cbe0", InetAddressHelper.toString(address));
    }

    @Test
    public void testIPv6AddressUncompressedScoped() throws UnknownHostException
    {
        final Inet6Address address = InetAddressHelper.getIPv6Address(
            bytes(0xfe, 0x80, 0x0, 0x1, 0x1, 0x0, 0x0, 0x1, 0x68, 0x86, 0x44, 0xff, 0xfe, 0xce, 0xa1, 0x8c),
            7
        );
        assertNotNull(address);
        assertArrayEquals(
            bytes(0xfe, 0x80, 0x0, 0x1, 0x1, 0x0, 0x0, 0x1, 0x68, 0x86, 0x44, 0xff, 0xfe, 0xce, 0xa1, 0x8c),
            address.getAddress()
        );
        assertEquals("fe80:1:100:1:6886:44ff:fece:a18c%7", address.getHostAddress());
        assertEquals("fe80:1:100:1:6886:44ff:fece:a18c%7", InetAddressHelper.toString(address));
    }

    @Test
    public void testIPv6AddressCompressedScoped() throws UnknownHostException
    {
        final Inet6Address address = InetAddressHelper.getIPv6Address(
            bytes(0xfe, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x68, 0x86, 0x44, 0xff, 0xfe, 0xce, 0xa1, 0x8c),
            7
        );
        assertNotNull(address);
        assertArrayEquals(
            bytes(0xfe, 0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x68, 0x86, 0x44, 0xff, 0xfe, 0xce, 0xa1, 0x8c),
            address.getAddress()
        );
        assertEquals("fe80:0:0:0:6886:44ff:fece:a18c%7", address.getHostAddress());
        assertEquals("fe80::6886:44ff:fece:a18c%7", InetAddressHelper.toString(address));
    }
}
