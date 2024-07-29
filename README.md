# Scott's Project Euler Software

## Current Build Status
Every commit triggers an automatic workflow on an ubuntu machine remotely.

### build status
![Main Project Build Status](https://github.com/meowFlute/project_euler/actions/workflows/makefile.yml/badge.svg)

### unit test status
I don't have unit tests yet (I've only just made this repo today) so this is just a placeholder

## Build Instructions
`make` in the root project directory should take care of it assuming you have the correct library dependencies available

### Dependencies
#### libc (math)
I'm not sure if this will come up, but I do link against the libc math library `-lm`. If that is an issue for someone, I probably won't be able to figure out how to help you.

#### GNU Multi Precision Library (GMP)
At the time of writing I'm using GMP 6.3.0 for some of the problems. This was more out of curiosity than need, and I used a few problems to learn about GMP syntax and practice using the library. 

I suggest following the [install instructions in the GMP documentation](https://gmplib.org/manual/Installing-GMP), but I am installing [during my build workflow](https://github.com/meowFlute/project_euler/blob/main/.github/workflows/makefile.yml). The flow there is something like this:

```console
wget -v https://ftp.gnu.org/gnu/gmp/gmp-6.3.0.tar.xz
tar -xf gmp-6.3.0.tar.xz
cd gmp-6.3.0/
./configure
make
sudo make install
```

#### primesieve
[primesieve](https://github.com/kimwalisch/primesieve) is a library by Kim Walisch that is [distributed through a ton of package managers](https://github.com/kimwalisch/primesieve). In my case I get it with a simple `sudo apt install libprimesieve-dev` during [my build workflow](https://github.com/meowFlute/project_euler/blob/main/.github/workflows/makefile.yml).

## What is this repo?
Just solving the problems at https://projecteuler.net/ for fun. I'll do it in C and/or Fortran and use it as an excuse to do some things in CUDA and OpenCL because that is part of the fun.
