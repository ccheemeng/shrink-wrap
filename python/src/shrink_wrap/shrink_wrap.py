import pathlib
import subprocess
from typing import TextIO


def shrink_wrap(
    input_dir: str,
    output_name: str,
    alpha: float,
    offset: float,
    timeout: float | None = None,
    stdout: TextIO | None = None,
    stderr: TextIO | None = None,
) -> None:
    """Shrink wraps all OBJ meshes in the directory `input`.

    Args:
        input_dir: Directory that contains OBJ meshes to be shrink wrapped.
        output_name: Target name of the OBJ and MTl shrink wrap files. For
            example, to write to "data/output/mit_teapot/teapot.obj" and
            "data/output/mit_teapot/teapot.mtl", use
            "data/output/mit_teapot/teapot".
        alpha: Alpha parameter. `alpha` must be greater than 0.0. See
            https://doc.cgal.org/latest/Alpha_wrap_3/index.html#title5.
        offset: Alpha parameter. `offset` must be greater than 0.0. See
            https://doc.cgal.org/latest/Alpha_wrap_3/index.html#title5.
        timeout: Optional timeout in seconds.
        stdout: Text file to write the process's standard output stream to.
        stderr: Text file to write the process's standard error stream to.

    Raises:
        FileNotFoundError: The required shrink_wrap binary was not installed.
        RuntimeError: The process exited with a non-zero return code.
        TimeoutError: A `timeout` was specified and the process timed out.
    """
    binary_path = pathlib.Path(__file__).parent.joinpath("bin", "shrink_wrap")
    if not binary_path.is_file():
        raise FileNotFoundError(f"Required binary at {binary_path} not found")
    try:
        subprocess.run(
            [str(binary_path), input_dir, output_name, str(alpha), str(offset)],
            capture_output=True,
            timeout=timeout,
            check=True,
            text=True,
            stdout=stdout,
            stderr=stderr,
        )
    except subprocess.CalledProcessError as e:
        raise RuntimeError(f"Return code {e.returncode}: {e.stderr}") from e
    except subprocess.TimeoutExpired as e:
        raise TimeoutError(f"Timeout after {e.timeout} s: {e.stderr}") from e
