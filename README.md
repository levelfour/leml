leML
====
leML is **l**lvm **e**xtended **M**eta **L**anguage.

## Dependencies

The environment is

* flex 2.5.39
* bison 2.7.12
* llvm 3.6.2
* clang 7.0.2

libraries are

* libncurses ver 5.4.0
* libz ver 1.0.0

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

If `leml` is executed with no options, `leml` takes `stdin` as input and emit LLVM IR to `stdout`.
