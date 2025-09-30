# ShrinkWrap  

This utility generates a watertight and orientable mesh over input 3D geometries via the [CGAL library](https://doc.cgal.org/latest/Alpha_wrap_3/index.html).
This mesh termed a "shrink wrap".  

### Input  

One or more polygon soups in one of the following file formats:  
* `.off`  
* `.obj`  
* `.stl`  
* `.ply`  
* `.ts`  
* `.vtp`  

### Output  

A shrink wrap in a file format corresponding to the input  

## Installation  

### Conda Linux  

1. Create a conda environment with the necessary preqrequisites via the `environment.yml` file:  

    ```bash
    conda env create -f environment.yml
    conda activate shrink-wrap
    ```
    
2. Build and compile the program:  

    ```bash
    cmake -S . -B ./build-linux
    make -C ./build-linux
    ```
    
3. Run the program:  

    ```bash
    ./shrink_wrap <alpha> <offset> -i <input1> -i <input2> ... # see Arguments below
    ```
    
## Arguments  

| Name       | Flags | Type          | Description                                                                                                                                                                                                                                                                          | Required | Default         |
|------------|-------|---------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|----------|-----------------|
| `alpha`    | -     | `double`      | Alpha parameter (see [CGAL 3D Alpha Wrapping](https://doc.cgal.org/latest/Alpha_wrap_3/index.html)), must be greater than 0                                                                                                                                                          | `true`   | -               |
| `offset`   | -     | `double`      | Offset parameter (see [CGAL 3D Alpha Wrapping](https://doc.cgal.org/latest/Alpha_wrap_3/index.html)), must be greater than 0                                                                                                                                                         | `true`   | -               |
| `input`    | `-i`  | `std::string` | Input filename; use one flag per input file                                                                                                                                                                                                                                          | `true`   | -               |
| `relative` | -     | `bool`        | If used, the alpha and offset used are the maximum diagonal length of the orthogonal box bounding the input divided by `alpha` and `offset` respectively                                                                                                                             | `false`  | `false`         |
| `ratio`    | `-r`  | `double`      | If used and `0.0 < ratio <= 1.0`, the shrink wrap is simplified until the ratio of the number of edges in the simplified mesh to the number of edges in the shrink wrap is equal to `ratio`                                                                                          | `false`  | `-1.0`          |
| `policy`   | `-p`  | `std::string` | If one of `"cp"`, `"ct"`, `"pp"`, or `"pt"`, uses a Garland-Heckbert "Classic Plane", Garland-Heckbert triangle-based, Trettner and Kobbelt "Probabilistic Plane", or Trettner and Kobbelt "Probabilistic Triangle" simplification strategy. Defaults to `"cp"` if `ratio` is valid. | `false`  | `""`            |
| `out`      | `-o`  | `std::string` | Output filename; if not provided this is derived from the first input filename and other parameters                                                                                                                                                                                  | `false`  | See description |