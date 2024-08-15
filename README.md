![image](https://github.com/user-attachments/assets/afb7a921-8212-49ef-bd53-27451ceac182)
![image](https://github.com/user-attachments/assets/8813e75f-125e-49d0-8e46-0f9a302180c3)

# Grid Generator

## Overview

The Grid Generator is a C++ project that provides functionality for generating and rendering different types of grids, including rectangular and triangular grids. 
The project uses the SFML library for rendering the grids and includes custom geometrical operations for grid manipulation.

## Features

* **Rectangular Grid Generation**: Create and manipulate rectangular grids with given example.
* **Triangular Grid Generation**: Create and manipulate a delanauy triangulation of points (Triangular grid is WIP).
* **Rendering**: Render the given grids using the SFML library.
* **Custom Geometry Handling**: Implement custom geometry operations for grid cells.

## Project structure

* **main.cpp**: The entry point of the application. It initializes and runs the grid generator.
* **rectangular.cpp**: Implementation of rectangular grid operations.
* **rectangular.h**: Header file for the rectangular grid operations.
* **renderer.cpp**: Contains the implementation of the rendering logic using SFML.
* **renderer.h**: Header file for the rendering operations.
* **triangular.cpp**: Implementation of triangular grid operations.
* **triangular.h**: Header file for the triangular grid operations.
* **geometry.h**: Contains the geometric operations used across different grid types.
* **CMakeLists.txt**: The CMake build configuration file, which sets up the project, specifies the C++ standard, and links necessary libraries.

## Dependencies

* **SFML (Simple and Fast Multimedia Library)**: Used for rendering. The project includes SFML as an external dependency.

## Building the Project

1. Clone the Repository:
```bash
git clone Vetterli65/grid_generator
cd grid_generator
```
2. Build a project
```bash
mkdir build-linux
cd build
cmake ..
make
```
3. Run exectuable
```bash
./grid_generator
```

## Usage

Upon running the executable, the application will generate and display a grid based on the configuration in the source files. 
The rendering will be handled using SFML, and you can manipulate the grid by modifying the source code.
Currently there are two examples to be choosen from in the **main.cpp**.

## References

* S. W. Sloan, "A fast algorithm for constructing Delaunay triangulations in the plane"
* P.G.A. Cizmas, "COMPUTATIONAL FLUID DYNAMICS FOR AEROSPACE APPLICATIONS"
