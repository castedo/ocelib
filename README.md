ocelib
======

Open Castedo Ellerman Library (including Interactive Brokers features)

This library contains C++ code that is very generic (thus open-sourced) and a
current dependency of proprietary projects of Castedo Ellerman (mainly on SUSE
Linux). Everything in this repository is meant to be public domain and is
thus permissively licensed via an MIT License.

This generic open source could be migrated into separate libraries. Contact me if
you are interested in sharing, collaborating, want more documentation or have
feedback. 

Functionality
-------------

* [SWIG](http://www.swig.org) type maps for boost::posix_time::ptime in R, Python and Java are in the
`swig` directory.
* The code under `jios` implements abstract interfaces for reading/writing
json-like input/output. There are concrete classes for outputting single-line or
pretty-printed JSON text. There are also functions for
converting, printing, parsing between Google Protocol Buffer messages and objects
implementing these `jios` interfaces.
* The code under `proto` has some Google Protocol Buffer headers for some
 Interactive Brokers API protocol messages.

Dependencies
------------

* Typical Linux development environment (make, gcc, etc...)
* C++11
* [CMake](http://www.cmake.org) >= 2.8.1, maybe lower
* [Boost](http://www.boost.org) >= 1.49.0, maybe lower
* [Google Protocol Buffers](http://developers.google.com/protocol-buffers/)
* [SWIG](http://www.swig.org) 
* [R](http://www.r-project.org)

Quick Start
-----------

From within a `ocelib` cloned working directory:

```
mkdir build
cd build
cmake ..
make
ctest -V
```

