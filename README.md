# Scott's Project Euler Software

## What is this repo?
Solving the problems at https://projecteuler.net/ for fun. I'll do it in C and/or Fortran and use it as an excuse to do some things in CUDA and OpenCL because that is part of the fun.

### Command-line Usage
argp was used to create a command-line interface, with `./project_euler --help` showing the familiar help formatting most CLI programs use.

```console
$ ./project_euler --help
Usage: project_euler [OPTION...]
project_euler -- a runnable set of solutions to the problems found on
projecteuler.net. The version number denotes <main version number>.<max problem
completed>.<sub version number>. This means that you can't run a problem
greater than whatever is in the <max problem complete> field of the version
number

  -a, --all                  run all of the problems
  -n, --no_numeric_only      report only numbers for solutions
  -p, --problems=CSV_PROBS   comma-separated (no spaces) integers representing
                             problems to run.e.g. 1,5,23
  -s, --problem_statement    include problem statements in the results
  -t, --time                 track and report CPU time of each problem solution
                            
  -x, --natural_language     report full-length text solutions in natural
                             language
  -z, --tabulated            report solutions in a table
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <mscottchristensen@gmail.com>.
```

Notably, the tabulated output option generates console-width outputs (as long as your console is sufficiently wide for the fixed-width fields) and handles wrapping of the problem statements and natural-language responses.

```console
$ ./project_euler -ztsp 1
| num | exe time    | answer | statement                                                                          |
===================================================================================================================
| 001 | 0.007000 ms | 233168 | If we list all the natural numbers below 10 that are multiples of 3 or 5, we get 3 |
|     |             |        | , 5, 6, and 9. The sum of these multiples is 23. Find the sume of all the multiple |
|     |             |        | s of 3 or 5 below 1000.                                                            |
-------------------------------------------------------------------------------------------------------------------
$ ./project_euler -ztsxp 1
| num | exe time    | answer | language                                | statement                                |
===================================================================================================================
| 001 | 0.007000 ms | 233168 | The sum of all multiples of 3 or 5 belo | If we list all the natural numbers below |
|     |             |        | w 1000 is 233168                        |  10 that are multiples of 3 or 5, we get |
|     |             |        |                                         |  3, 5, 6, and 9. The sum of these multip |
|     |             |        |                                         | les is 23. Find the sume of all the mult |
|     |             |        |                                         | iples of 3 or 5 below 1000.              |
-------------------------------------------------------------------------------------------------------------------
$ ./project_euler -ztp 1
| num | exe time    | answer |
==============================
| 001 | 0.007000 ms | 233168 |
------------------------------
```

### Solving New Problems
I added an automation script in the src directory that will work in most linux environments. It is a simple shell script with usage and help documentation baked in, but the gist of it is you just run it with a problem number that is one larger than the `HIGHEST_PROBLEM_COMPLETED` parameter in main.c

So for example, to create the files for problem 9 and add the function pointers and references to the new header file into the right places, all I had to do was run `./add_problem.sh -p 9` from the src/ directory. This did four main things:
1. Created a problem_009.h file and filled it with the right info according to the naming standards I've been following
2. Created a problem_009.c file and filled it with enough to run -- references the header, implements the function, etc.
3. Adds the header to project_euler.h
4. Adds the function pointer to main.c and increments the `HIGHEST_PROBLEM_COMPLETED` parameter while also setting the `SUB_VERSION` parameter to `0`

Takes some of the tedium out of working on a new problem

### TODO LIST
- Add in some kind of unit testing suite that checks against the example problems given in most questions as well as the solutions
- Add options for different kinds of output (natural language, tabulated, numeric only responses, etc.)

## Current Build Status
Every commit triggers a couple of automatic workflows on virtual ubuntu machines remotely using github actions. These workflows run in parallel.

### `make minimal`
The primary workflow first install the dependencies listed below, then builds using `make minimal` and reports any errors via the status below as well as email alerts to my email address. 

### `make lint`
A secondary linting workflow installs dependencies as well as splint, then runs `make lint` which uses splint to generate lint.err and lint.out files. The Makefile entry is written such that it ignores errors and simply stores them for later inspection. Both files are available for download as zipped artifacts for every build.

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
