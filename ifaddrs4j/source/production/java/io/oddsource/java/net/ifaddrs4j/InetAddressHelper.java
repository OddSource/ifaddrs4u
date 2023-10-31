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
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Objects;

/**
 * A collection of helpers to make creating and using {@link Inet4Address} and {@link Inet6Address}
 * easier and more closely matching expectations.
 *
 * @since 1.0.0
 */
public final class InetAddressHelper
{
    /**
     * The number of bytes contained in an IPv4 address.
     */
    public static final short IPV4_DATA_LENGTH = 4;

    /**
     * The number of bytes contained in an IPv6 address.
     */
    public static final short IPV6_DATA_LENGTH = 16;

    /**
     * The number of words contained in an IPv6 address.
     */
    public static final short IPV6_WORDS = IPV6_DATA_LENGTH / 2;

    private static final short BYTE_SIZE = 8;

    private static final short BYTE_MASK = 0xff;

    private static final int WORD_MASK = 0xffff;

    private InetAddressHelper()
    {
        throw new UnsupportedOperationException();
    }

    /**
     * Construct an IPv4 address from the 4 given bytes.
     *
     * @param data Exactly 4 bytes
     * @return the IPv4 address represented by these 4 bytes.
     * @throws UnknownHostException if the bytes are malformed in some way.
     * @throws IllegalArgumentException if more or fewer than 4 bytes are supplied.
     */
    public static Inet4Address getIPv4Address(final byte[] data) throws UnknownHostException
    {
        InetAddressHelper.validateDataLength(data, InetAddressHelper.IPV4_DATA_LENGTH, "IPv4");
        return (Inet4Address) InetAddress.getByAddress(data);
    }

    /**
     * Construct an IPv6 address from the 16 given bytes.
     *
     * @param data Exactly 16 bytes
     * @param scope_id The numeric scope ID, if the address is scoped, or null otherwise
     * @return the IPv6 address represented by these 16 bytes.
     * @throws UnknownHostException if the bytes are malformed in some way.
     * @throws IllegalArgumentException if more or fewer than 16 bytes are supplied.
     */
    public static Inet6Address getIPv6Address(final byte[] data, final Integer scope_id) throws UnknownHostException
    {
        InetAddressHelper.validateDataLength(data, InetAddressHelper.IPV6_DATA_LENGTH, "IPv6");
        return Inet6Address.getByAddress(null, data, Objects.requireNonNullElse(scope_id, -1));
    }

    /**
     * Get the dotted-decimal notation of the given IPv4 address. This is just a wrapper around
     * {@link Inet4Address#getHostAddress()}, but it's given to be consistent with {@link #toString(Inet6Address)}.
     *
     * @param address The IPv4 address
     * @return the dotted-decimal notation of the given IPv4 address.
     */
    public static String toString(final Inet4Address address)
    {
        return address.getHostAddress();
    }

    /**
     * Get the RFC 5952 compliant hexadecimal words notation of the given IPv6 address. This differs from the
     * undesirable implementation of {@link Inet6Address#getHostAddress()} in that it will compact the longest
     * sequence of consecutive zeroes into two colons ({@code ::}) in compliance with RFC 5952, while the Java
     * standard implementation will not do this. E.g., the loopback will stringify to {@code ::1} in this
     * method, while the Java standard implementation will result in {@code 0:0:0:0:0:0:0:1}.
     *
     * @param address The IPv6 address
     * @return the compressed hexadecimal words notation of the given IPv6 address.
     */
    // note: CyclomaticComplexity ... 19, max allowed 10 ... but there is nothing simple about stringifying IPv6
    @SuppressWarnings({"checkstyle:MethodLength", "checkstyle:CyclomaticComplexity"})
    public static String toString(final Inet6Address address)
    {
        final byte[] data = address.getAddress();
        final int[] words = new int[IPV6_WORDS];
        for(short b = 0, w = 0; b < IPV6_DATA_LENGTH; b += 2, w++)
        {
            words[w] = ((data[b] & BYTE_MASK) << BYTE_SIZE) + (data[b + 1] & BYTE_MASK);
        }

        short tempZeroesStart = -1;
        short tempZeroesEnd = -1;
        short zeroesStart = -1;
        short zeroesEnd = -1;
        boolean endWithColon = false;
        for(short w = 0; w < IPV6_WORDS; w++)
        {
            if(words[w] == 0)
            {
                if(tempZeroesStart < 0)
                {
                    tempZeroesStart = w;
                }
                tempZeroesEnd = w;
            }
            else if(tempZeroesStart >= 0)
            {
                if(zeroesStart < 0 || tempZeroesEnd - tempZeroesStart > zeroesEnd - zeroesStart)
                {
                    zeroesStart = tempZeroesStart;
                    zeroesEnd = tempZeroesEnd;
                }
                tempZeroesStart = -1;
                tempZeroesEnd = -1;
            }
        }
        if(tempZeroesStart >= 0)
        {
            if(zeroesStart < 0 || tempZeroesEnd - tempZeroesStart > zeroesEnd - zeroesStart)
            {
                zeroesStart = tempZeroesStart;
                zeroesEnd = tempZeroesEnd;
                endWithColon = true;
            }
        }

        final var builder = new StringBuilder();
        for(short w = 0; w < IPV6_WORDS; w++)
        {
            if(zeroesStart >= 0 && w >= zeroesStart && w <= zeroesEnd)
            {
                if(w == zeroesEnd)
                {
                    builder.append(':');
                }
                continue;
            }
            if(w > 0)
            {
                builder.append(':');
            }
            builder.append(Integer.toHexString(words[w] & WORD_MASK));
        }
        if(endWithColon)
        {
            builder.append(':');
        }
        if(address.getScopeId() >= 1)
        {
            builder.append('%').append(address.getScopeId());
        }

        return builder.toString();
    }

    /**
     * A generic method that can be used on an instance of an InetAddress subclass without knowing its exact
     * type (or with generics like {@code <T extends InetAddress>}). See {@link #toString(Inet4Address)} and
     * {@link #toString(Inet6Address)} for more information.
     *
     * @param address The {@link Inet4Address} or {@link Inet6Address}
     * @return the string representation of the given address.
     */
    public static String toString(final InetAddress address)
    {
        if(address instanceof Inet4Address)
        {
            return InetAddressHelper.toString((Inet4Address) address);
        }
        else if(address instanceof Inet6Address)
        {
            return InetAddressHelper.toString((Inet6Address) address);
        }
        else
        {
            throw new IllegalArgumentException(
                "New, unrecognized subclass of InetAddress that did not exist as of Java 20"
            );
        }
    }

    private static void validateDataLength(final byte[] data, final short length, final String type)
    {
        if(data.length != length)
        {
            throw new IllegalArgumentException(
                type + " address data must be " + length + " bytes in length, not " + data.length + " bytes."
            );
        }
    }
}
