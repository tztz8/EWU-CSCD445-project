# EWU CSCD445 Project

Conway games of life on a cubes surface

## Table of Content

-   [EWU CSCD445 Project](#ewu-cscd445-project)
    -   [Table of Content](#table-of-content)
    -   [Team:](#team)
    -   [Conway games of life on a cubes surface](#conway-games-of-life-on-a-cubes-surface)
    -   [Functions](#functions)
    -   [Min Goal](#min-goal)
    -   [Report](#report)
        -   [How to run/use](#how-to-runuse)
        -   [Sample run](#sample-run)
        -   [SpeedUp](#speedup)
        -   [Video](#video)
    -   [Making the program](#making-the-program)
        -   [For the program](#for-the-program)
        -   [For Makefile](#for-makefile)
    -   [Notes](#notes)

## Team:

-   `Timbre Freeman` : Email <tfreeman3@ewu.edu>
-   `Nicholas Gainer`: Email <ngainer@ewu.edu>
-   `Jeremy Munson`: Email <jmunson3@ewu.edu>
-   `Johnathan Smith` : Email <jsmith245@ewu.edu>

## Conway games of life on a cubes surface

Each face of a cube will have a 2d grid of Conway games of life and their edges will interact with the connected face’s

## Functions

1.  OpenGL Cube
    1.  If we have time also have input that can effect Conway games of life as it runs
2.  CPU Conway games of life
    1.  If we have time
3.  CUDA Conway, games of life
    1.  But include edges interacting
4.  Convert each 2d grid of Conway games of life to image for the face on the cube

## Min Goal

At min, a cube with each face running Conway games of life on CUDA that has the edges interact with some start state to see it run (Ex have some Glider’s)

## Report

### How to run/use

BLANK

### Sample run

BLANK

### SpeedUp

BLANK

### Video

TODO Video Link

## Making the program

We only test on linux

### For the program

Need OpenGL lib and dev

Need GLEW lib and dev

Need GLU lib and dev

Need GLM dev

Need `git` clone [sub modules](.gitmodules)

### For Makefile

Need `CMake`

Need `pandoc` and `wkhtmltopdf`

Need `nvcc`

Wants `clang`

May Need `gcc`

## Notes

-   [OpenGL Code base off](https://github.com/tztz8/OpenGL-All)
-   [Graphics Interoperability](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__INTEROP.html)
-   [OpenGL Interoperability](https://docs.nvidia.com/cuda/cuda-runtime-api/group__CUDART__OPENGL.html)
