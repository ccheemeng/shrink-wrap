package io.github.ccheemeng.shrink_wrap;

import java.io.IOException;
import java.nio.file.Path;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

public class ShrinkWrap {
  public static void shrinkWrap(
      String inputDir,
      String outputName,
      double alpha,
      double offset,
      long timeout)
      throws IOException,
          InterruptedException,
          RuntimeException,
          TimeoutException {
    Path binaryPath = Loader.getBinary();
    Process process =
        new ProcessBuilder(
                binaryPath.toString(),
                inputDir,
                outputName,
                String.valueOf(alpha),
                String.valueOf(offset))
            .start();
    boolean exited = process.waitFor(timeout, TimeUnit.SECONDS);
    if (!exited) {
      process = process.destroyForcibly();
      throw new TimeoutException(
          String.format("%s timeout after %d s", process.toString(), timeout));
    }
    int returnCode = process.exitValue();
    if (returnCode != 0) {
      throw new RuntimeException(
          String.format("%s return code %d", process.toString(), returnCode));
    }
    return;
  }
}
