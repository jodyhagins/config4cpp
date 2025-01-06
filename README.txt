This repository is NOT the official place for Config4Cpp, and it is not a fork,
as there is no official repository that I can find.  The original source was
pulled from www.config4star.org, and put into this github repository.

I committed the original version 1.2 sources, then added some mininal cmake
stuff to get it to build.  I then have made some minor changes that make it
a bit easier for me to use.  I'm not opposed to using modern C++ and the
standard library.

Currently, I don't care about performance much when processing configuration
stuff, but the original, and especially the usability extensions are not
very performant.  One day I may do a rewrite.  However, there are not a lot
of good existing tests, and I'm a bit trepidatious about doing so.

I use cmake with FetchContent.  For example,

    message(STATUS "Processing third-party config4cpp...")
    FetchContent_Declare(
        config4cpp
        GIT_REPOSITORY https://github.com/jodyhagins/config4cpp.git
        GIT_TAG main
        SYSTEM)
    FetchContent_MakeAvailable(config4cpp)


Everything below is from the original readme.


Documentation
-------------

Comprehensive documentation for Config4Cpp is available. However, it is
distributed separately from the source code. You can find the
documentation (in PDF and HTML formats) on www.config4star.org.


Compilation instructions
------------------------

The build system has been tested on: (1) Linux with G++, (2) Cygwin with
G++, and (3) Windows with Visual C++ 2013

To build on Linux or Cygwin, do the following:
	1. If you are compiling on Linux, then add the config4cpp/lib
	   directory into the LD_LIBRARY_PATH environment variable, so the
	   shared library can be located.

	2. Run "make" to build with optimization, or "make BUILD_TYPE=debug"
	   for a debug build. I recommend building with optimization.

To build on Windows with Visual C++, run the following commands:

	vcvars32.bat
	nmake -f Makefile.win

If you are building on another operating system, or with another
compiler, the you might need to edit "Makefile.inc" (if you are on a
UNIX-like operating system) or "Makefile.win.inc" (if you are on
Windows). You might also need to edit "src/platform.h" and
"src/platform.cpp".

Executables will be put into the "bin" directory, and library files will
be put into the "lib" directory.
