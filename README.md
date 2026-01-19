# ShrinkWrap  

This utility generates a watertight and orientable mesh over input 3D geometries 
via the [CGAL library](https://doc.cgal.org/latest/Alpha_wrap_3/index.html). 
This mesh is termed a "shrink wrap".  

ShrinkWrap takes as inputs and outputs `.obj` files, and maps materials if available.  

A shrink wrap of one storey of an apartment building:  
| Input | Output |
| - | - |
| ![Tembusu L12](./img/Tembusu%20L12.png) | ![Tembusu L12 wrap](./img/Tembusu%20L12%20wrap.png) |

A shrink wrap from multiple inputs:  
| Input 1 | Input 2 | Output |
| - | - | - |
| ![KPF Robinson Tower Structure](./img/KPF%20Robinson%20Tower/KPF%20Robinson%20Tower%20Structure.png) | ![KPF Robinson Tower Facade](./img/KPF%20Robinson%20Tower/KPF%20Robinson%20Tower%20Facade.png) | ![KPF Robinson Tower wrap](./img/KPF%20Robinson%20Tower/KPF%20Robinson%20Tower%20wrap.png) |

## Installation  

### Obtain software  

#### Download binary  

1. Download the shrink wrap binary from the latest release in this repo.
1. Place it in a `build-linux/` directory and rename it to `shrink_wrap`, or replace the run commands below to the appropriate location of the binary.

#### Build from source  

1. Create a conda environment with the necessary preqrequisites via the 
`environment.yml` file:  

    ```bash
    conda env create -f environment.yml
    conda activate shrink-wrap
    ```

1. Build and compile the program:  

    ```bash
    cmake -S . -B ./build-linux
    cmake --build ./build-linux
    ```

### Run the program  

    ```bash
    # Standard usage
    ./build-linux/shrink_wrap 500 1 -i ./data/Tembusu\ L12.obj

    # Multiple inputs
    cd ./data/KPF\ Robinson\ Tower
    ../../build-linux/shrink_wrap .5 .001 -i KPF\ Robinson\ Tower\ Facade\ Lower\ Left\ 2.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Back\ 3.obj -i KPF\ Robinson\ Tower\ 23-25F.obj -i KPF\ Robinson\ Tower\ 26-27F.obj -i KPF\ Robinson\ Tower\ 8F.obj -i KPF\ Robinson\ Tower\ 18-19F.obj -i KPF\ Robinson\ Tower\ 2-3F.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Right\ 1.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Left\ 5.obj -i KPF\ Robinson\ Tower\ 6F.obj -i KPF\ Robinson\ Tower\ Facade\ Lower\ Right\ 3.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Front\ 2.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Right\ 2.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Left\ 2.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Right\ 4.obj -i KPF\ Robinson\ Tower\ 4-5F.obj -i KPF\ Robinson\ Tower\ Facade\ Ground.obj -i KPF\ Robinson\ Tower\ 12-14F.obj -i KPF\ Robinson\ Tower\ 7F.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Back\ 1.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Back\ 4.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Right\ 6.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Left\ 4.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Left\ 6.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Back\ 2.obj -i KPF\ Robinson\ Tower\ Facade\ Lower\ Front.obj -i KPF\ Robinson\ Tower\ Facade\ Lower\ Left\ 1.obj -i KPF\ Robinson\ Tower\ Facade\ Lower\ Right\ 2.obj -i KPF\ Robinson\ Tower\ 28-32F.obj -i KPF\ Robinson\ Tower\ 9-11F.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Left\ 7.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Left\ 1.obj -i KPF\ Robinson\ Tower\ Facade\ Lower\ Right\ 1.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Left\ 3.obj -i KPF\ Robinson\ Tower\ 1F.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Right\ 5.obj -i KPF\ Robinson\ Tower\ Basement.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Front\ 1.obj -i KPF\ Robinson\ Tower\ Facade\ Lower\ Left\ 3.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Right\ 3.obj -i KPF\ Robinson\ Tower\ 20-22F.obj -i KPF\ Robinson\ Tower\ Facade\ Upper\ Front\ 3.obj -i KPF\ Robinson\ Tower\ 15-17F.obj
    ```

## Arguments  

| Name | Flags | Type | Description | Option Required | Argument Required | Default |
|-|-|-|-|-|-|-|
| `alpha` | - | `double` | Alpha parameter (see [CGAL 3D Alpha Wrapping](https://doc.cgal.org/latest/Alpha_wrap_3/index.html)), must be greater than 0 | Positional argument | - | - |
| `offset` | - | `double` | Offset parameter (see [CGAL 3D Alpha Wrapping](https://doc.cgal.org/latest/Alpha_wrap_3/index.html)), must be greater than 0 | Positional argument | - | - |
| `input` | `-i` | `std::string` | Input filename; use one flag per input file | `true` | `true` | - |
| `relative` | `-r` | `bool` | If used, the alpha and offset used are the maximum diagonal length of the orthogonal box bounding the input divided by `alpha` and `offset` respectively | `false` | `true` | `false` |
| `out` | `-o` | `std::string` | Output filename; if not provided this is derived from the first input filename and other parameters | `false` | `true` | See description |
