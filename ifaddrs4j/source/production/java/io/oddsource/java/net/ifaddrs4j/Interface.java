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
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.UUID;

/**
 * Represents a network interface (adapter) on the system, including its index, name, UUID if on Windows,
 * configuration flags, configured MTU, MAC (hardware) address, IPv4 address(es), and IPv6 address(es).
 *
 * @since 1.0.0
 */
public final class Interface
{
    private static final boolean IS_WINDOWS = System.getProperty("os.name").toLowerCase(Locale.US).contains("win");

    private static final String NO_WINDOWS = "The windowsUuid property is supported only on Windows platforms.";

    private final int index;

    private final String name;

    private final UUID windowsUuid;

    private final int flags;

    private final Long mtu;

    private final MacAddress macAddress;

    private final List<InterfaceIPAddress<Inet4Address>> ipv4Addresses;

    private final List<InterfaceIPAddress<Inet6Address>> ipv6Addresses;

    Interface(
        final int index,
        final String name,
        final int flags,
        final Long mtu,
        final MacAddress macAddress,
        final List<InterfaceIPAddress<Inet4Address>> ipv4Addresses,
        final List<InterfaceIPAddress<Inet6Address>> ipv6Addresses
    )
    {
        this(index, name, null, flags, mtu, macAddress, ipv4Addresses, ipv6Addresses);
    }

    Interface(
        final int index,
        final String name,
        final String windowsUuid,
        final int flags,
        final Long mtu,
        final MacAddress macAddress,
        final List<InterfaceIPAddress<Inet4Address>> ipv4Addresses,
        final List<InterfaceIPAddress<Inet6Address>> ipv6Addresses
    )
    {
        if (windowsUuid != null && Interface.IS_WINDOWS)
        {
            throw new UnsupportedOperationException(Interface.NO_WINDOWS);
        }

        this.index = index;
        this.name = name;
        this.windowsUuid = windowsUuid == null ? null : UUID.fromString(windowsUuid);
        this.flags = flags;
        this.mtu = mtu;
        this.macAddress = macAddress;
        this.ipv4Addresses = Collections.unmodifiableList(ipv4Addresses);
        this.ipv6Addresses = Collections.unmodifiableList(ipv6Addresses);
    }

    /**
     * Get the interface index.
     *
     * @return the interface index.
     */
    public int getIndex()
    {
        return this.index;
    }

    /**
     * Get the interface name.
     *
     * @return the interface name.
     */
    public String getName()
    {
        return this.name;
    }

    /**
     * Get the interface UUID on the Windows platform. If this platform is not Windows, this method
     * throws {@link UnsupportedOperationException}.
     *
     * @return the Windows platform interface UUID.
     * @throws UnsupportedOperationException if the platform is not Windows.
     */
    public UUID getWindowsUuid()
    {
        if (Interface.IS_WINDOWS)
        {
            throw new UnsupportedOperationException(Interface.NO_WINDOWS);
        }
        return this.windowsUuid;
    }

    /**
     * Checks whether the indicated flag is enabled for this interface. See the documentation for
     * {@link InterfaceFlag} for more information.
     *
     * @param flag The flag to check
     * @return whether the flag is enabled.
     */
    public boolean isFlagEnabled(final InterfaceFlag flag)
    {
        return (this.flags & flag.flag) == flag.flag;
    }

    /**
     * Returns the interface's configured MTU, or {@code null} if no MTU is configured.
     *
     * @return the MTU or {@code null}.
     */
    public Long getMtu()
    {
        return this.mtu;
    }

    /**
     * Get the MAC address (hardware address) for this interface, or {@code null} if none is configured.
     *
     * @return the MAC address or {@code null}.
     */
    public MacAddress getMacAddress()
    {
        return this.macAddress;
    }

    /**
     * Get an immutable iterator of the IPv4 addresses configured on this interface.
     *
     * @return an unmodifiable iterator over the interface's IPv4 addresses.
     */
    public Iterator<InterfaceIPAddress<Inet4Address>> getIpv4Addresses()
    {
        return new AddressIterator<>(this.ipv4Addresses.iterator());
    }

    /**
     * Get an immutable iterator of the IPv6 addresses configured on this interface.
     *
     * @return an unmodifiable iterator over the interface's IPv6 addresses.
     */
    public Iterator<InterfaceIPAddress<Inet6Address>> getIpv6Addresses()
    {

        return new AddressIterator<>(ipv6Addresses.iterator());
    }

    @Override
    public String toString()
    {
        final var builder = new StringBuilder(this.name);
        builder.append(" (").append(this.index).append("): flags=").append(Integer.toHexString(this.flags)).append('<');
        if(this.flags > 0)
        {
            int i = 0;
            for(final var flag : InterfaceFlag.values())
            {
                if((this.flags & flag.flag) == flag.flag)
                {
                    if(i++ > 0)
                    {
                        builder.append(',');
                    }
                    builder.append(flag.display);
                }
            }
        }
        builder.append('>');
        if(this.mtu != null)
        {
            builder.append(" mtu ").append(this.mtu);
        }
        builder.append('\n');
        if(this.macAddress != null)
        {
            builder.append("        ether ").append(this.macAddress).append('\n');
        }
        for(var address : this.ipv4Addresses)
        {
            builder.append("        inet  ").append(address).append('\n');
        }
        for(var address : this.ipv6Addresses)
        {
            builder.append("        inet6 ").append(address).append('\n');
        }
        return builder.toString();
    }

    private static class AddressIterator<InterfaceIPAddress> implements Iterator<InterfaceIPAddress>
    {
        private final Iterator<InterfaceIPAddress> iterator;

        AddressIterator(final Iterator<InterfaceIPAddress> iterator)
        {
            this.iterator = iterator;
        }

        @Override
        public boolean hasNext()
        {
            return this.iterator.hasNext();
        }

        @Override
        public InterfaceIPAddress next()
        {
            return this.iterator.next();
        }
    }
}
