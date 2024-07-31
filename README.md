# Scott's Project Euler Software

## What is this repo?
Solving the problems at https://projecteuler.net/ for fun. I'll do it in C and/or Fortran and use it as an excuse to do some things in CUDA and OpenCL because that is part of the fun.

### TODO LIST
- Make a script that automates adding new problems. something like `./addproblem 72` and it will go in and edit/create all of the files appropriately so that I don't have to do repetitive tasks.
- Add in some kind of unit testing suite that checks against the example problems given in most questions as well as the solutions

## Current Build Status
Every commit triggers an automatic workflow on an ubuntu machine remotely.

### build status
![Main Project Build Status](https://github.com/meowFlute/project_euler/actions/workflows/makefile.yml/badge.svg)

## Build Instructions
`make` in the root project directory should take care of it assuming you have the correct library dependencies available. If you don't use ctags or any of the other things that I have baked into my standard `make` call, you can run `make minimal` which at the moment is what is running in my [my build workflow](https://github.com/meowFlute/project_euler/blob/main/.github/workflows/makefile.yml)

### Dependencies
#### libc (math)
I'm not sure if this will come up, but I do link against the libc math library `-lm`. If that is an issue for someone, I probably won't be able to figure out how to help you.

#### GNU Multi Precision Library (GMP)
At the time of writing I'm using GMP 6.3.0 for some of the problems. This was more out of curiosity than need, and I used a few problems to learn about GMP syntax and practice using the library. 

I suggest following the [install instructions in the GMP documentation](https://gmplib.org/manual/Installing-GMP), but I am installing [during my build workflow](https://github.com/meowFlute/project_euler/blob/main/.github/workflows/makefile.yml). Because my build flow runs on an ubuntu machine I'm leveraging the libgmp-dev apt package by installing with `sudo apt install libgmp-dev`

#### primesieve
[primesieve](https://github.com/kimwalisch/primesieve) is a library by Kim Walisch that is [distributed through a ton of package managers](https://github.com/kimwalisch/primesieve). In my case I get it with a simple `sudo apt install libprimesieve-dev` during [my build workflow](https://github.com/meowFlute/project_euler/blob/main/.github/workflows/makefile.yml).
