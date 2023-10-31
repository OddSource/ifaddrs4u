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

import java.util.Arrays;

/**
 * Represents a network interface's MAC address (aka hardware address).
 */
public final class MacAddress
{
    static
    {
        if(!Loader.LOADED)
        {
            // this will never actually happen, but we need to ensure that Loader initializes
            throw new RuntimeException("Not possible!");
        }
    }

    private final String representation;

    private final byte[] data;

    private final short data_length;

    /**
     * Constructs the MAC address / hardware address from the given String representation, which should be
     * at least 6 and at most 8 hexadecimal-formatted bytes separated by hyphens or colons.
     *
     * @param representation The string representation
     */
    public MacAddress(final String representation)
    {
        this(representation, MacAddress.getDataFromRepr(representation));
    }

    /**
     * Constructs the MAC address / hardware address from the given array of bytes, which should contain
     * at least 6 and at most 8 bytes.
     *
     * @param data The bytes making up the address
     */
    public MacAddress(final byte[] data)
    {
        this(MacAddress.getReprFromData(data), Arrays.copyOf(data, data.length));
    }

    MacAddress(final String representation, final byte[] data)
    {
        this.representation = representation;
        this.data = data;
        this.data_length = (short) data.length;
    }

    private static native byte[] getDataFromRepr(String representation);

    private static native String getReprFromData(byte[] data);

    /**
     * Gets the string representation of the MAC address (e.g. a6:83:e7:2e:a1:67).
     *
     * @return the representation.
     */
    public String getRepresentation()
    {
        return this.representation;
    }

    /**
     * Gets the address bytes. This data is typically 6 bytes long, but can be as much as
     * 8 bytes for some interfaces on Windows platforms.
     *
     * @return the address bytes.
     */
    public byte[] getData()
    {
        return Arrays.copyOf(this.data, this.data_length);
    }

    /**
     * Gets the number of address bytes, typically 6, but can be as much as 8 for some interfaces
     * on Windows platforms.
     *
     * @return the number of address bytes.
     */
    public short getLength()
    {
        return this.data_length;
    }

    /**
     * Returns the same as {@link #getRepresentation()}.
     *
     * @return the representation.
     */
    @Override
    public String toString()
    {
        return this.representation;
    }

    @Override
    public int hashCode()
    {
        return Arrays.hashCode(this.data);
    }

    /**
     * Tests whether this MAC address equals the other, given MAC address.
     *
     * @param other The other address
     * @return whether the addresses are equal.
     */
    public boolean equals(final MacAddress other)
    {
        return Arrays.equals(this.data, other.data);
    }

    @Override
    public boolean equals(final Object other)
    {
        return other instanceof MacAddress && this.equals((MacAddress) other);
    }
}
