# CIRE: C++ Incremental rigorous error-analyser

CIRE is similar to [SATIRE](<add link to repo>) but in C++. This project was meant to provide an error analysis tool and
library for C/C++ programs. Ideally it should have all the capabilities of Satire and
more.

# Dependencies

CIRE requires the following softwares installed on your system

* ibex-lib > 2.8.9
  * [Github](https://github.com/ibex-team/ibex-lib)
  * [Installation instructions](http://ibex-team.github.io/ibex-lib/install.html)
* python2.x > 2.7 (Ibex scripts are currently not compatible with python3)
* g++
* gcc
* bison
* flex
* cmake

### If you want to use the LLVM frontend
* LLVM > 16

## IBEX installation
### Linux and MacOS
Download `ibex-lib-ibex-2.8.9.tar.gz` (the tar file) from [here](https://github.com/ibex-team/ibex-lib/releases/tag/ibex-2.8.9)
```bash
tar xvfz ibex-2.8.9.tgz
cd ibex-2.8.9
sudo ./waf configure --enable-shared
sudo ./waf install
```

To Uninstall
```bash
sudo ./waf uninstall
sudo ./waf distclean
```

# Building

The CMakeLists.txt file is configured to build the library and the executable in the build directory.
Make sure the shared library files are in your PATH. If not, set the LD_LIBRARY_PATH environment variable to where your
shared library files are located.
Run the following

```bash
mkdir build-debug
cmake CIRE
```

## Building the LLVM frontend

To build the LLVM frontend, you need to have LLVM installed on your system. Set LT_LLVM_INSTALL_DIR to the directory
where LLVM is installed.

```bash
cmake CIRE_LLVM -DLT_LLVM_INSTALL_DIR=<path to LLVM install directory>
```

# Usage

The executable is located in the build-debug directory. Run the following to see CIRE run on an example file

```bash
LD_LIBRARY_PATH=/usr/local/lib
./build-debug/CIRE ./benchmarks/addition/addition.txt
```

## LLVM Frontend
