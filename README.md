# Algorithmen und Datenstrukturen
This is a C library that contains various implementations of interesting algorithms and data structures.

## Contents
* [AVL Tree](inc/AvlTree.h)
* [HyperLogLog](inc/HyperLogLog.h)

## Makefile targets
### `make all`
Compiles the library into an archive. The output file is called _libaud.a_.

### `make test`
Creates the unit test executable. The output file is called _utest_.

### `make doc`
Creates html documentation. The main page is located in _doc/html/index.html_.

**NOTE:** [_Doxygen_](http://www.stack.nl/~dimitri/doxygen/) has to be installed on your system. If you want to see
call- and include-graphs in the generated documentation, you also have to have [_Graphviz_](http://www.graphviz.org/)
installed.

### `make clean`
Deletes all build files (object files and executables).

### `make destroy`
Deletes everything make has ever created (includes downloaded libraries and documentation). After running this command,
your repo should look as if it was freshly cloned from Github.

## Usage
Include the headers in the _inc/_ folder and link the archive _libaud.a_ (`make all`) to your project.

Link with `-lm`.
