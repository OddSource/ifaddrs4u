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

import java.io.IOException;
import java.util.Properties;
import java.util.logging.Logger;

/**
 * Reveals version info about the Java library and its C++ extension.
 *
 * @since 1.0
 */
public final class VersionInfo
{
    static
    {
        if (!Loader.LOADED)
        {
            // this will never actually happen, but we need to ensure that Loader initializes
            throw new RuntimeException("Not possible!");
        }
    }

    private static final Logger LOGGER = Logger.getLogger(VersionInfo.class.getCanonicalName());

    private static String VERSION;

    private static short VERSION_MAJOR;

    private static short VERSION_MINOR;

    private static short VERSION_PATCH;

    private static String VERSION_SUFFIX;

    private VersionInfo()
    {
    }

    private static synchronized void load()
    {
        if (VersionInfo.VERSION == null)
        {
            try (final var stream = VersionInfo.class.getResourceAsStream("version.properties"))
            {
                final var properties = new Properties();
                properties.load(stream);

                VersionInfo.VERSION = properties.getProperty("ifaddrs4j.version");
                VersionInfo.VERSION_MAJOR = Short.parseShort(properties.getProperty("ifaddrs4j.version.major"));
                VersionInfo.VERSION_MINOR = Short.parseShort(properties.getProperty("ifaddrs4j.version.minor"));
                VersionInfo.VERSION_PATCH = Short.parseShort(properties.getProperty("ifaddrs4j.version.patch"));
                VersionInfo.VERSION_SUFFIX = properties.getProperty("ifaddrs4j.version.suffix");

                if (!VersionInfo.VERSION.equals(VersionInfo.getExtensionVersion()))
                {
                    VersionInfo.LOGGER.warning(String.format(
                        "C++ extension version %s does not match Java library version %s",
                        VersionInfo.getExtensionVersion(),
                        VersionInfo.VERSION
                    ));
                }
                else if (
                    !VersionInfo.VERSION_SUFFIX.equals(VersionInfo.getExtensionSuffix()) ||
                    VersionInfo.VERSION_MAJOR != VersionInfo.getExtensionMajor() ||
                    VersionInfo.VERSION_MINOR != VersionInfo.getExtensionMinor() ||
                    VersionInfo.VERSION_PATCH != VersionInfo.getExtensionPatch()
                )
                {
                    VersionInfo.LOGGER.warning(String.format(
                        "C++ extension version info (%d, %d, %d, '%s') does not match " +
                        "Java library version (%d, %d, %d, '%s')",
                        VersionInfo.getExtensionMajor(), VersionInfo.getExtensionMinor(),
                        VersionInfo.getExtensionPatch(), VersionInfo.getExtensionSuffix(),
                        VersionInfo.VERSION_MAJOR, VersionInfo.VERSION_MINOR,
                        VersionInfo.VERSION_PATCH, VersionInfo.VERSION_SUFFIX
                    ));
                }
            }
            catch (IOException | IllegalArgumentException e)
            {
                VersionInfo.VERSION = VersionInfo.VERSION_SUFFIX = "load-error";
                VersionInfo.VERSION_MAJOR = VersionInfo.VERSION_MINOR = VersionInfo.VERSION_PATCH = -1;
            }
        }
    }

    /**
     * Get the Java library version string.
     *
     * @return the version string.
     */
    public static String getJavaVersion()
    {
        VersionInfo.load();
        return VersionInfo.VERSION;
    }

    /**
     * Get the Java library major version.
     *
     * @return the major version.
     */
    public static short getJavaMajor()
    {
        VersionInfo.load();
        return VersionInfo.VERSION_MAJOR;
    }

    /**
     * Get the Java library minor version.
     *
     * @return the minor version.
     */
    public static short getJavaMinor()
    {
        VersionInfo.load();
        return VersionInfo.VERSION_MINOR;
    }

    /**
     * Get the Java library patch version.
     *
     * @return the patch version.
     */
    public static short getJavaPatch()
    {
        VersionInfo.load();
        return VersionInfo.VERSION_PATCH;
    }

    /**
     * Get the Java library version suffix, or null if there is no suffix.
     *
     * @return the version suffix or null.
     */
    public static String getJavaSuffix()
    {
        VersionInfo.load();
        return VersionInfo.VERSION_SUFFIX;
    }

    /**
     * Get the C++ extension version string.
     *
     * @return the version string.
     */
    public static native String getExtensionVersion();

    /**
     * Get the C++ extension major version.
     *
     * @return the major version.
     */
    public static native short getExtensionMajor();

    /**
     * Get the C++ extension minor version.
     *
     * @return the minor version.
     */
    public static native short getExtensionMinor();

    /**
     * Get the C++ extension patch version.
     *
     * @return the patch version.
     */
    public static native short getExtensionPatch();

    /**
     * Get the C++ extension version suffix, or null if there is no suffix.
     *
     * @return the version suffix or null.
     */
    public static native String getExtensionSuffix();
}
