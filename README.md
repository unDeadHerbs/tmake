T-Diagram Make
==============

A tool for deciding the build order of chained compilers and cross compilers.

T-Diagrams
----------

A T-Diagram is a mental tool for keeping track of how [a set of
compiler tools are
built](https://www.youtube.com/watch?v=PjeE8Bc96HY).  This is
particularly useful when working on cross compilers or building up the
base tool for a new system.

If you have an assembler written in machine code.
```
            +-----------------------------+
Assembler = | Assembly            Binary  |
            +---------+          +--------+
                      |  Binary  |
                      +----------+
```

And a compiler to assembly written in assembly.
```
           +------------------------------+
Compiler = | Language            Assembly |
           +---------+          +---------+
                     | Assembly |
                     +----------+
```

One can put the Compiler though the Assembler to obtain a compiler
that is executable on your system.
```
            +------------------------------+                        +------------------------------+
Compiler2 = | Language            Assembly |                      = | Language            Assembly |
            +---------+          +---------+--------------------+   +---------+          +---------+
                      | Assembly | Assembly             Binary  |             |  Binary  |
                      +----------+---------+          +---------+             +----------+
                                           |  Binary  |
                                           +----------+
```


Future Work
-----------

1) Currently an optimizer isn't expressible in the TDig class.

```
            +--------------------------------------+
Optimizer = | Intermediate  Quality   Intermediate |
            +-------------+          +-------------+
                          |  Binary  |
                          +----------+
```

2) Support for interpreters.

```
              +------------------------+
Interpreter = |   Language             |
              +-------------+          +
                            |  Binary  |
                            +----------+
```

3) Support for naming programs, i.e. a seperate file that lists the T Diageam type of each program.

4) If a directory is untyped look for a file inside with the same base name as use it's type.

5) Write an intelligent solver. Rather than the rrivial one and the brute force ones that currently exist.
