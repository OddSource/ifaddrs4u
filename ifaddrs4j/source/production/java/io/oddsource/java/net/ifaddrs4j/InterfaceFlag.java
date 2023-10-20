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

/**
 * Represents bitwise flags associated with the interface's current state.<br>
 * <br>
 * The bitwise flag values represented by the {@link #flag} field will differ from platform to platform,
 * and you must not assume their being the same from one platform to the next.<br>
 * <br>
 * Not every flag is supported or available on every system. Notably, if the enum constant's {@link #flag}
 * value equals the static constant {@link #UNSUPPORTED_FLAG}, it should not be used.
 */
public enum InterfaceFlag
{
    /**
     * The interface is up.
     */
    IsUp(getConstant("IsUp"), "UP"),
    /**
     * The interface is running.
     */
    IsRunning(getConstant("IsRunning"), "RUNNING"),
    /**
     * This is a loopback interface.
     */
    IsLoopback(getConstant("IsLoopback"), "LOOPBACK"),
    /**
     * This interface is a point-to-point interface with a destination address set and (typically)
     * no prefix length and no broadcast or multicast support.
     */
    IsPointToPoint(getConstant("IsPointToPoint"), "POINTTOPOINT"),
    /**
     * At least one broadcast address is set on this interface.
     */
    BroadcastAddressSet(getConstant("BroadcastAddressSet"), "BROADCAST"),
    /**
     * This interface supports multicast traffic.
     */
    SupportsMulticast(getConstant("SupportsMulticast"), "MULTICAST"),
    /**
     * Debugging is enabled for this interface.
     */
    DebugEnabled(getConstant("DebugEnabled"), "DEBUG"),
    /**
     * Promiscuous mode is enabled for this interface.
     */
    PromiscuousModeEnabled(getConstant("PromiscuousModeEnabled"), "PROMISC"),
    /**
     * This interface is configured to receive all multicast packets, even those for multicast groups
     * for which no application is bound.
     */
    ReceiveAllMulticastPackets(getConstant("ReceiveAllMulticastPackets"), "ALLMULTI"),
    /**
     * No Address Resolution Protocol is available for this interface (see
     * <a href="https://en.wikipedia.org/wiki/Address_Resolution_Protocol">
     * https://en.wikipedia.org/wiki/Address_Resolution_Protocol</a>).
     */
    NoARP(getConstant("NoARP"), "NOARP"),
    /**
     * Obsolete: Avoid the use of trailers (though most interfaces with standard routable IP addresses
     * still support this, aka "SMART").
     */
    NoTrailers(getConstant("NoTrailers"), "SMART"),
    /**
     * A transmission is in progress on this interface (*-BSD only).
     */
    TransmissionInProgress(getConstant("TransmissionInProgress"), "OACTIVE"),
    /**
     * This interface can't hear its own transmissions (*-BSD only).
     */
    Simplex(getConstant("Simplex"), "SIMPLEX"),
    /**
     * This is a master interface with, or eligible to have, one or more slave interfaces (Linux only).
     */
    Master(getConstant("Master"), "MASTER"),
    /**
     * This is a slave interface (Linux only).
     */
    Slave(getConstant("Slave"), "SLAVE");

    /**
     * A sentinel to indicate whether a given flag is supported on the current platform.
     */
    public static final int UNSUPPORTED_FLAG = getConstant("UNSUPPORTED_FLAG");

    /**
     * The value used for bitwise operations.
     */
    public final int flag;

    /**
     * The string used to display this flag in {@link Interface#toString()}.
     */
    public final String display;

    InterfaceFlag(final int flag, final String display)
    {
        this.flag = flag;
        this.display = display;
    }

    private static int getConstant(final String name)
    {
        return EnumConstantsHelper.getInterfaceFlagConstant(name);
    }
}
