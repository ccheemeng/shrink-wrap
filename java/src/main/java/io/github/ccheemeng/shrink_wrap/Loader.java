package io.github.ccheemeng.shrink_wrap;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.net.URISyntaxException;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.nio.file.attribute.PosixFilePermission;
import java.security.ProtectionDomain;
import java.util.Enumeration;
import java.util.Optional;
import java.util.Set;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

class Loader {
  private static final String LIB_DIR = "bin/linux-x86_64";
  private static final String BINARY_NAME = "shrink_wrap";
  private static final String TEMP_DIR_PREFIX = "shrink-wrap";
  private static final Path BINARY_PATH;

  static {
    try {
      BINARY_PATH = Loader.install();
    } catch (IllegalStateException | IOException | URISyntaxException e) {
      throw new ExceptionInInitializerError(e);
    }
  }

  public static Path getBinary() {
    return Loader.BINARY_PATH;
  }

  private static Path install()
      throws IllegalStateException, IOException, URISyntaxException {
    JarFile jarFile = Loader.getJarFile();
    Path jarBinaryPath = Path.of(LIB_DIR).resolve(BINARY_NAME);
    JarEntry jarBinaryEntry =
        Optional.<Path>of(Path.of(LIB_DIR).resolve(BINARY_NAME))
            .map(path -> jarFile.getJarEntry(path.toString()))
            .orElseThrow(() ->
                new IllegalStateException(
                    String.format(
                        "Required binary at %s not found",
                        jarBinaryPath.toString())));
    Path tempDir = Files.createTempDirectory(TEMP_DIR_PREFIX);
    Path targetBinary = tempDir.resolve(BINARY_NAME);
    try (InputStream in = jarFile.getInputStream(jarBinaryEntry)) {
      Files.copy(in, targetBinary, StandardCopyOption.REPLACE_EXISTING);
      targetBinary = Loader.setOwnerExecute(targetBinary);
    }

    Enumeration<JarEntry> jarEntries = jarFile.entries();
    while (jarEntries.hasMoreElements()) {
      JarEntry jarEntry = jarEntries.nextElement();
      if (jarEntry.isDirectory()) {
        continue;
      }
      Optional<Path> entryPath = Optional.<Path>of(Path.of(jarEntry.getName()));
      String entryParent =
          entryPath
              .map(path -> path.getParent())
              .map(parentPath -> parentPath.toString())
              .orElse("");
      String entryBasename =
          entryPath
              .map(path -> path.getFileName())
              .map(fileName -> fileName.toString())
              .orElseThrow(() ->
                  new IllegalStateException(
                      String.format(
                          "Could not get filename of %s",
                          jarEntry.toString())));
      if (!entryParent.equals(LIB_DIR) || entryBasename.equals(BINARY_NAME)) {
        continue;
      }
      Path targetLib = tempDir.resolve(entryBasename);
      try (InputStream in = jarFile.getInputStream(jarEntry)) {
        Files.copy(in, targetLib, StandardCopyOption.REPLACE_EXISTING);
        Loader.setOwnerExecute(targetLib);
      }
    }

    return targetBinary.toAbsolutePath();
  }

  private static JarFile getJarFile()
      throws IllegalArgumentException, IOException, URISyntaxException {
    URL jarUrl =
        Optional.<ProtectionDomain>of(Loader.class.getProtectionDomain())
            .map(protectionDomain -> protectionDomain.getCodeSource())
            .map(codeSource -> codeSource.getLocation())
            .orElseThrow(() ->
                new IllegalStateException("Could not get URL of JAR file"));
    URI jarUri = jarUrl.toURI();
    JarFile jarFile = new JarFile(new File(jarUri));
    return jarFile;
  }

  private static Path setOwnerExecute(Path path) throws IOException {
    Set<PosixFilePermission> permissions = Files.getPosixFilePermissions(path);
    permissions.add(PosixFilePermission.OWNER_EXECUTE);
    Path newPath = Files.setPosixFilePermissions(path, permissions);
    return newPath;
  }
}
