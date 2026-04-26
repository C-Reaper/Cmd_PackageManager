# Project README

## Overview
The project appears to be a simple C-based application that uses the `PackageManager` library for managing packages. The `main.c` file initializes and loads the `PackageManager`, which is responsible for handling package operations based on configuration files in specified directories.

## Features
- **Package Management**: Loads and manages packages using configurations from YAML files.
- **YAML Parsing**: Utilizes a YAML parser to read and process configuration data.
- **Directory Operations**: Performs directory operations to locate and load package-related files.

## Project Structure
The project contains the following files and directories:

### Prerequisites
- C/C++ Compiler (e.g., GCC, Clang)
- Make utility
- Standard development tools

## Build & Run
The build process uses `Makefile.linux` for Linux environments. The application can be built using the following commands:

```sh
cd <Project>
make -f Makefile.linux all
```

To clean and rebuild the project, use:

```sh
make -f Makefile.linux clean
make -f Makefile.linux all
```

For building and executing the project, you can use:

```sh
make -f Makefile.linux exe
```

This will compile the source code and produce an executable file in the `build/` directory.