leML
====
leML is **l**lvm **e**xtended **M**eta **L**anguage.

## Dependencies

The environment is

* flex 2.5.39
* bison 2.7.12
* llvm 3.6.2

libraries are

* libncurses ver 5.4.0
* libz ver 1.0.0

## Install

### Mac OS

Use Homebrew.

```
brew install flex bison llvm
```

### Linux (Ubuntu)

```
apt-get install flex bison clang-3.6 lldb-3.6 llvm bc
```

`bc` is needed for test.

Before running `make`, change the content of `Makefile.in` appropriately.
Recommendation is

```
CXX=clang++-3.6
CC=clang-3.6
LLVMCONFIG=llvm-config-3.6
```

Check if these above are installed in `/usr/bin`.

## Usage

### build

```
make
```

to build.

### options

* `-jit` run the program on jit instead of emit code
* `-o [filename]` specify input file name
* `-v` verbose output
* `-nostdlib` does not link built-in library automatically
* `-mem2reg` apply `mem2reg` Pass (default is off)

If `leml` is executed with no options, `leml` takes `stdin` as input and emit LLVM IR to `stdout`.

### built-in library

Built-in libraries are located in `lib/External`. By default, there is `\_\_builtins.c`, and this includes built-in functions needed to run raytracer.

### test

```
./run_test.sh
```

test case existing in `./test` will be executed on LLVM JIT.
