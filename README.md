# Algorithmen und Datenstrukturen
This is a C library that contains various implementations of interesting algorithms and data structures.

## Contents
* [AVL Tree](src/AvlTree.h)

## Makefile targets
### `make all`
Since this is a library, `make all` only creates the executable for the unit tests. The output file is called `aud`.

### `make doc`
Creates html documentation. The main page is located in _doc/html/index.html_.

**NOTE:** [_Doxygen_](http://www.stack.nl/~dimitri/doxygen/) has to be installed on your system.

### `make clean`
Deletes all build files (object files and executables).

### `make destroy`
Deletes everything make has ever created (includes downloaded libraries and documentation). After running this command,
your repo should look as if it was freshly cloned from Github.