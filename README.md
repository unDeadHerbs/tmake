T-Diagram Make
==============

Usage: tmake [results] [-r runnable]
Results in the form from.to.in
Runnable defaults to bin, sh

Example:
$ ls
null.hello.cpp cpp.bin.bin
$ tmake null.hello.bin
cpp.bin.bin null.hello.cpp

Example:
$ ls
cpp.bin.bin bf.cpp.cpp null.hello.bf
$ tmake null.hello.bin
cpp.bin.bin bf.cpp.cpp
bf.cpp.bin null.hello.bf
cpp.bin.bin null.hello.cpp

Specs:
Will accept folders as input. Will accept multiple runnables.

Special rule for "make a.b.runnable".
Special type "null" means "no input" or "no output"
