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
import java.nio.file.FileSystemNotFoundException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.Locale;

/**
 * Helper that loads the dynamic JNI library from many possible locations in a platform-specific way.
 *
 * @since 1.0.0
 */
final class Loader
{
    public static final boolean LOADED;

    private static final String DYLIB = "dylib";

    private static final String SO = "so";

    private static final String DLL = "dll";

    private static final String DOT = ".";

    private static final String SPC = " ";

    static
    {
        final String osName = System.getProperty("os.name").toLowerCase(Locale.US);
        final String osVersion = System.getProperty("os.version").toLowerCase(Locale.US);
        final String osArch = System.getProperty("os.arch").toLowerCase(Locale.US);
        if (osName.contains("mac") || osName.contains("darwin"))
        {
            if (osArch.contains("arch64"))
            {
                loadLibrary("darwin-arm64", DYLIB);
            }
            else
            {
                loadLibrary("darwin-intel64", DYLIB);
            }
            LOADED = true;
        }
        else if(osName.contains("win"))
        {
            loadLibrary("win32", DLL);
            LOADED = true;
        }
        else if(osName.contains("nux"))
        {
            loadLibrary("rhel", SO);
            LOADED = true;
        }
        else
        {
            LOADED = false;
            throw new RuntimeException("Unsupported operating system " + osName + SPC + osVersion + SPC + osArch);
        }
    }

    private Loader()
    {
        throw new UnsupportedOperationException();
    }

    static private void loadLibrary(final String platform, final String extension)
    {
        final var options = new String[] {"libifaddrs4j-" + platform + DOT + extension, "libifaddrs4j." + extension};

        String foundName = null;
        URL url = null;
        for (final String fileName : options)
        {
            url = Loader.class.getResource("/" + fileName);
            if (url != null)
            {
                foundName = fileName;
                break;
            }
        }

        final File file;
        if (url != null)
        {
            file = Loader.urlToFile(url, foundName);
        }
        else
        {
            file = Loader.findFileOnPath(options);
        }

        if (file == null || !file.exists())
        {
            throw new RuntimeException(
                "Unable to find any file in candidate locations matching libifaddrs4j." + extension +
                " or libifaddrs4j-" + platform + DOT + extension + (file == null ? DOT : (" (extra: " + file + ")"))
            );
        }

        System.load(file.getAbsolutePath());
    }

    static private File urlToFile(final URL url, final String foundName)
    {
        try
        {
            return Paths.get(url.toURI()).toFile();
        }
        catch(final URISyntaxException | IllegalArgumentException | FileSystemNotFoundException | SecurityException e1)
        {
            File file = null;
            try
            {
                final var jar = Paths.get(
                    Loader.class.getProtectionDomain().getCodeSource().getLocation().toURI()
                ).toFile();
                file = new File(jar.getParentFile(), foundName);
            }
            catch(final URISyntaxException | IllegalArgumentException |
                        FileSystemNotFoundException | SecurityException e2)
            {
                try
                {
                    file = File.createTempFile("dyjni-", foundName);
                }
                catch(final IOException | IllegalArgumentException | SecurityException ignore)
                {
                }
            }
            if (file != null)
            {
                try(var stream = url.openStream())
                {
                    Files.copy(stream, file.toPath());
                }
                catch(final IOException | IllegalArgumentException | SecurityException ignore)
                {
                }
            }
            return file;
        }
    }

    static private File findFileOnPath(final String[] options)
    {
        for (final String path : System.getProperty("java.library.path").split(File.pathSeparator))
        {
            for (final String fileName : options)
            {
                final var temp = new File(path, fileName);
                if (temp.exists())
                {
                    return temp;
                }
            }
        }
        return null;
    }
}
