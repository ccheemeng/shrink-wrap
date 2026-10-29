# ShrinkWrap

This utility generates a watertight and orientable mesh over input 3D geometries via the [CGAL library](https://doc.cgal.org/latest/Alpha_wrap_3/index.html). This mesh is termed a "shrink wrap".

ShrinkWrap takes as inputs and outputs OBJ files, and maps materials if available.

A shrink wrap of one storey of an apartment building:
| Input | Output |
| - | - |
| ![Tembusu L12](./img/Tembusu%20L12.png) | ![Tembusu L12 wrap](./img/Tembusu%20L12%20wrap.png) |

A shrink wrap from multiple inputs:
| Input 1 | Input 2 | Output |
| - | - | - |
| ![KPF Robinson Tower Structure](./img/KPF%20Robinson%20Tower/KPF%20Robinson%20Tower%20Structure.png) | ![KPF Robinson Tower Facade](./img/KPF%20Robinson%20Tower/KPF%20Robinson%20Tower%20Facade.png) | ![KPF Robinson Tower wrap](./img/KPF%20Robinson%20Tower/KPF%20Robinson%20Tower%20wrap.png) |

## CLI Executable

### Build from source

1. Create a conda environment with the necessary preqrequisites via the
`environment.yml` file:

    ```bash
    conda env create -f environment.yml
    conda activate shrink-wrap
    ```

1. Build and compile the program:

    ```bash
    cmake -S . -B ./build
    cmake --build ./build
    ```

### Run

```bash
./build/shrink_wrap data/input/Tembusu\ L12/ data/output/Tembusu\ L12 500.0 1.0

./build/shrink_wrap data/input/KPF\ Robinson\ Tower data/output/KPF\ Robinson\ Tower 0.5 0.001
```

#### Arguments

| Name | Type | Description |
|-|-|-|
| `input` | `std::string` | Directory that contains OBJ meshes to be shrink wrapped |
| `out` | `std::string` | Target name of the OBJ and MTL shrink wrap files |
| `alpha` | `double` | Alpha parameter (see [CGAL 3D Alpha Wrapping](https://doc.cgal.org/latest/Alpha_wrap_3/index.html)), must be greater than 0.0 |
| `offset` | `double` | Offset parameter (see [CGAL 3D Alpha Wrapping](https://doc.cgal.org/latest/Alpha_wrap_3/index.html)), must be greater than 0.0 |

## Python

### Install from releases

Install the latest wheel from this repo's releases:

```bash
pip install <wheel-file>
```

### Build from source

1. Build a Python wheel with Docker:

    ```bash
    docker build --output=. .
    ```

1. Install the package from the built wheel:

    ```bash
    pip install <wheel-file>
    ```

### Usage

```python
import shrink_wrap

shrink_wrap.shrink_wrap(
    "data/input/Tembusu L12/", "data/output/Tembusu L12", 500.0, 1.0
)
shrink_wrap.shrink_wrap(
    "data/input/KPF Robinson Tower",
    "data/output/KPF Robinson Tower",
    0.5,
    0.001,
)
```
