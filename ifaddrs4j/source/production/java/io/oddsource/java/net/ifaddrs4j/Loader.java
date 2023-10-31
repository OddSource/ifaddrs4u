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

import java.io.File;
import java.io.IOException;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.nio.file.StandardCopyOption;
import java.util.Arrays;
import java.util.Locale;
import java.util.concurrent.atomic.AtomicReference;

/**
 * Helper that loads the dynamic JNI library from many possible locations using both platform-specific and generic
 * library file names.
 *
 * @since 1.0.0
 */
@SuppressWarnings("checkstyle:illegalcatch")
final class Loader
{
    public static final boolean LOADED;

    private static final String LIB = "libifaddrs4j";

    private static final String TMP_FILE_PREFIX = "jnitmp-";

    private static final String DYLIB = "dylib";

    private static final String SO = "so";

    private static final String DLL = "dll";

    private static final String DOT = ".";

    private static final String HYP = "-";

    private static final String SPC = " ";

    private static final String BSD = "bsd";

    private static final String ID_EQ = "id=";

    private static final String ID_LIKE_EQ = "id_like=";

    private static final String IA32 = "ia32";

    private static final String IA64 = "ia64";

    private static final String X86 = "x86";

    private static final String X64 = "x64";

    private static final String X86_64 = "x86_64";

    private static final String ARCH64 = "arch64";

    private static final String ARM = "arm";

    private static final String AMD64 = "amd64";

    private static final String OS_NAME = System.getProperty("os.name").toLowerCase(Locale.US);

    private static final String OS_VERSION = System.getProperty("os.version").toLowerCase(Locale.US);

    private static final String OS_ARCH = System.getProperty("os.arch").toLowerCase(Locale.US);

    private static final System.Logger logger;

    static
    {
        logger = System.getLogger(Loader.class.getCanonicalName());

        final var info = getKnownPlatformInfo();
        if(info != null)
        {
            logger.log(
                System.Logger.Level.DEBUG,
                "Identified natively-supported platform " + info.platform + HYP + info.architecture +
                " and will attempt to load the precompiled shared library for that platform"
            );
            loadLibrary(
                LIB + HYP + info.platform + HYP + info.architecture + DOT + info.extension,  // full expected name
                LIB + HYP + info.platform + DOT + info.extension, // possible name without architecture
                LIB + DOT + info.extension // backup name as a last-ditch effort
            );
        }
        else
        {
            logger.log(
                System.Logger.Level.DEBUG,
                "Identified natively-unsupported platform " + OS_NAME + SPC + OS_VERSION + SPC + OS_ARCH +
                ", but will look for externally-compiled shared library and attempt to load that"
            );
            try
            {
                // last-ditch effort in case the user has placed an externally-built library on their path
                loadLibrary(LIB + DOT + SO, LIB + DOT + DYLIB, LIB + DOT + DLL);
            }
            catch(final RuntimeException e)
            {
                LOADED = false;
                throw new RuntimeException(
                    "Unsupported operating system " + OS_NAME + SPC + OS_VERSION + SPC + OS_ARCH + ". File an issue " +
                    "at https://github.com/OddSource/ifaddrs4u/issues if you believe this is a bug or you would " +
                    "like to support additional operating systems.", e
                );
            }
        }
        LOADED = true;
    }

    private Loader()
    {
        throw new UnsupportedOperationException();
    }

    private static PlatformInfo getKnownPlatformInfo()
    {
        final String architecture = Loader.getArchitecture();
        if(architecture == null)
        {
            return null;
        }

        String platform = null;
        String extension = null;
        if(Loader.OS_NAME.contains("mac") || Loader.OS_NAME.contains("darwin"))
        {
            platform = "macos";
            extension = DYLIB;
        }
        else if(Loader.OS_NAME.contains("win"))
        {
            platform = "windows";
            extension = DLL;
        }
        else if(Loader.OS_NAME.contains("linux") || Loader.OS_NAME.contains("posix") ||
                Loader.OS_NAME.contains(Loader.BSD))
        {
            platform = Loader.getUnixVariant();
            extension = SO;
        }

        return platform != null ? new PlatformInfo(platform, architecture, extension) : null;
    }

    @SuppressWarnings("checkstyle:CyclomaticComplexity") // 11, maximum allowed is 10, but hard-pressed to simplify this
    private static String getUnixVariant()
    {
        final File file = new File("/etc/os-release");
        final var platform = new AtomicReference<String>();
        if(file.exists())
        {
            try
            {
                final String os_release = Files.readString(
                    file.toPath(),
                    StandardCharsets.UTF_8
                ).toLowerCase(Locale.US);
                Arrays.stream(os_release.split("\\r?\\n")).forEach(line -> {
                    if(line.startsWith(Loader.ID_EQ) && line.contains(Loader.BSD))
                    {
                        platform.set(Loader.BSD);
                    }
                    else if(line.startsWith(Loader.ID_LIKE_EQ) && (line.contains("debian") || line.contains("ubuntu")))
                    {
                        platform.set("linux-deb");
                    }
                    else if(line.startsWith(Loader.ID_LIKE_EQ) && (line.contains("fedora") || line.contains("rhel")))
                    {
                        platform.set("linux-el");
                    }
                });
            }
            catch(final IOException | RuntimeException | LinkageError e)
            {
                Loader.logger.log(
                    System.Logger.Level.WARNING,
                    "Expected " + OS_NAME + SPC + OS_VERSION + SPC + OS_ARCH + " to have /etc/os-release, but " +
                    "encountered exception attempting to read that file",
                    e
                );
            }
        }
        return platform.get();
    }

    private static String getArchitecture()
    {
        String architecture = null;
        if(Loader.OS_ARCH.contains(Loader.ARCH64))
        {
            architecture = Loader.ARM;
        }
        else if(Loader.OS_ARCH.contains(Loader.AMD64) || Loader.OS_ARCH.contains(Loader.X86_64) ||
                Loader.OS_ARCH.contains(Loader.IA64))
        {
            architecture = Loader.X64;
        }
        else if(Loader.OS_ARCH.contains(Loader.X86) || Loader.OS_ARCH.contains(Loader.IA32))
        {
            architecture = Loader.IA32;
        }
        return architecture;
    }

    private static void loadLibrary(final String... options)
    {
        String foundName = null;
        URL url = null;
        for(final String fileName : options)
        {
            url = Loader.class.getResource("/" + fileName);
            if(url != null)
            {
                foundName = fileName;
                break;
            }
        }

        final File file;
        if(url != null)
        {
            file = Loader.urlToFile(url, foundName);
        }
        else
        {
            file = Loader.findFileOnPath(options);
        }
        Loader.logSourceOfLibraryFile(url, file);

        if(file == null || !file.exists())
        {
            throw new RuntimeException(
                "Unable to find any shared library file through the classloader or on java.library.path matching " +
                "any of the following options: " + Arrays.toString(options) +
                (file == null ? DOT : (" (extra: " + file + ")"))
            );
        }

        System.load(file.getAbsolutePath());
    }

    private static void logSourceOfLibraryFile(final URL url, final File file)
    {
        if(file != null && Loader.logger.isLoggable(System.Logger.Level.DEBUG))
        {
            if(url != null)
            {
                if(url.toString().contains(file.toString()))
                {
                    Loader.logger.log(
                        System.Logger.Level.DEBUG,
                        "Successfully found libifaddrs4j shared library file " + file +
                        " directly on the classpath and will attempt to load it."
                    );
                }
                else
                {
                    Loader.logger.log(
                        System.Logger.Level.DEBUG,
                        "Successfully found libifaddrs4j shared library URL " + url + " within the JAR file " +
                        "and copied it to " + file + ", will attempt to load it."
                    );
                }
            }
            else
            {
                Loader.logger.log(
                    System.Logger.Level.DEBUG,
                    "Successfully found libifaddrs4j shared library file at " + file +
                    " and will attempt to load it."
                );
            }
        }
    }

    private static File urlToFile(final URL url, final String foundName)
    {
        try
        {
            return Paths.get(url.toURI()).toFile();
        }
        catch(final URISyntaxException | RuntimeException e1)
        {
            File file;
            try
            {
                file = new File(Paths.get(
                    Loader.class.getProtectionDomain().getCodeSource().getLocation().toURI()
                ).toFile(), TMP_FILE_PREFIX + foundName);
                Loader.scheduleForDeletion(file, foundName);
                Loader.copyStream(url, file);
            }
            catch(final URISyntaxException | IOException | RuntimeException e2)
            {
                try
                {
                    file = File.createTempFile(TMP_FILE_PREFIX, foundName);
                    Loader.scheduleForDeletion(file, foundName);
                    Loader.copyStream(url, file);
                }
                catch(final IOException | RuntimeException e3)
                {
                    file = null;
                }
            }
            return file;
        }
    }

    private static void copyStream(final URL from, final File to) throws IOException
    {
        try(var stream = from.openStream())
        {
            Files.copy(stream, to.toPath(), StandardCopyOption.REPLACE_EXISTING);
        }
    }

    private static void scheduleForDeletion(final File file, final String name)
    {
        try
        {
            file.deleteOnExit();
        }
        catch(final SecurityException e4)
        {
            Loader.logger.log(
                System.Logger.Level.WARNING,
                "Created temp file " + file + " to hold ifaddrs4j dynamic library " + name +
                " found in JAR file, but a security manager prevents us from scheduling it for deletion " +
                "upon JVM exit. It will hang around forever until manually deleted."
            );
        }
    }

    private static File findFileOnPath(final String[] options)
    {
        for(final String path : System.getProperty("java.library.path").split(File.pathSeparator))
        {
            for(final String fileName : options)
            {
                final var temp = new File(path, fileName);
                if(temp.exists())
                {
                    return temp;
                }
            }
        }
        return null;
    }

    private static final class PlatformInfo
    {
        final String platform;

        final String architecture;

        final String extension;

        PlatformInfo(final String platform, final String architecture, final String extension)
        {
            this.platform = platform;
            this.architecture = architecture;
            this.extension = extension;
        }
    }
}
