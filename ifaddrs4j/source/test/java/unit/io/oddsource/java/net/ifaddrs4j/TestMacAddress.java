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

import org.junit.Test;

public class TestMacAddress
{
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
    public void testConstructWithString()
    {
        MacAddress address = new MacAddress("a4:83:e7:2e:a1:67");

        assertEquals(address.toString(), "a4:83:e7:2e:a1:67");
        assertEquals(address.getRepresentation(), "a4:83:e7:2e:a1:67");
        assertEquals(address.getLength(), 6);
        assertArrayEquals(address.getData(), bytes(0xa4, 0x83, 0xe7, 0x2e, 0xa1, 0x67));

        address = new MacAddress("b2:fb:b8:5b:84:e8:fe:ff");

        assertEquals(address.toString(), "b2:fb:b8:5b:84:e8:fe:ff");
        assertEquals(address.getRepresentation(), "b2:fb:b8:5b:84:e8:fe:ff");
        assertEquals(address.getLength(), 8);
        assertArrayEquals(address.getData(), bytes(0xb2, 0xfb, 0xb8, 0x5b, 0x84, 0xe8, 0xfe, 0xff));
    }

    @Test
    public void testConstructWithData()
    {
        MacAddress address = new MacAddress(bytes(0x67, 0xa1, 0x2e, 0xff, 0xa4, 0xe7, 0xe3));

        assertEquals(address.toString(), "67:a1:2e:ff:a4:e7:e3");
        assertEquals(address.getRepresentation(), "67:a1:2e:ff:a4:e7:e3");
        assertEquals(address.getLength(), 7);
        assertArrayEquals(address.getData(), bytes(0x67, 0xa1, 0x2e, 0xff, 0xa4, 0xe7, 0xe3));

        address = new MacAddress(bytes(0x67, 0xaa, 0xa1, 0x2e, 0xff, 0xa4, 0xe7, 0xbb));

        assertEquals(address.toString(), "67:aa:a1:2e:ff:a4:e7:bb");
        assertEquals(address.getRepresentation(), "67:aa:a1:2e:ff:a4:e7:bb");
        assertEquals(address.getLength(), 8);
        assertArrayEquals(address.getData(), bytes(0x67, 0xaa, 0xa1, 0x2e, 0xff, 0xa4, 0xe7, 0xbb));
    }

    @Test
    public void testConstructMalformed()
    {
        assertThrows(IllegalArgumentException.class, () -> new MacAddress("67:a1:2e:ff:e5"));
        assertThrows(IllegalArgumentException.class, () -> new MacAddress("67:a1:2e:ff:e5:e6:e7:e8:e9"));
        assertThrows(IllegalArgumentException.class, () -> new MacAddress("67a12ef1a4e7"));

        assertThrows(
            IllegalArgumentException.class,
            () -> new MacAddress(bytes(0x67, 0xa1, 0x2e, 0xff, 0xa4))
        );
        assertThrows(
            IllegalArgumentException.class,
            () -> new MacAddress(bytes(0x67, 0xa1, 0x2e, 0xff, 0xa4, 0xff, 0xa4, 0xe7, 0xbb))
        );
    }

    @Test
    public void testEquals()
    {
        assertEquals(new MacAddress("a4:83:e7:2e:a1:67"), new MacAddress("a4:83:e7:2e:a1:67"));
        assertEquals(new MacAddress("a4:83:e7:2e:a1:67"), new MacAddress("A4:83:E7:2E:A1:67"));
        assertNotEquals(new MacAddress("b2:fb:b8:5b:84:e8"), new MacAddress("b2:fb:b8:5b:84:e8:ff"));
        assertNotEquals(new MacAddress("b2:fb:b8:5b:84:e8:fe"), new MacAddress("b2:fb:b8:5b:84:e8:ff"));
    }
}
