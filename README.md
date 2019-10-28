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
            +------------------------------+                          +------------------------------+
Compiler2 = | Language            Assembly |                        = | Language            Assembly |
            +---------+          +---------+--------------------+     +---------+          +---------+
                      | Assembly | Assembly             Binary  |               |  Binary  |
                      +----------+---------+          +---------+               +----------+
                                           |  Binary  |
                                           +----------+
```


Future Work
-----------

Currently an optimizer isn't expressible in the TDig class.

```
            +--------------------------------------+
Optimizer = | Intermediate  Quality   Intermediate |
            +-------------+          +-------------+
                          |  Binary  |
                          +----------+
```
