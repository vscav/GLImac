# GLImac

> OpenGL 3+ practical course.

## Linux installation

Update package database :

```
$ sudo apt update
```

Install the updates identified previously :

```
$ sudo apt upgrade
```

Install g++ (C++ compiler) :

```
$ sudo apt install g++
```

Install CMake :

```
$ sudo apt install cmake
```

Install GLEW :

```
$ sudo apt install libglew-dev
```

Install SDL (1.2 version) :

```
$ sudo apt install libsdl1.2-dev
```

Install mesa-utils :

```
$ sudo apt install mesa-utils
```

This last package provides various simple GL utilities built for Mesa, such as glxinfo and glxgears.
For example, you can check your current version of OpenGL :

```
$ glxinfo | grep 'version'
```

## Usage

In the directory containing the GLImac folder, create the build folder and move into it :

```
$ mkdir GLImac-build && cd GLImac-build
```

Build and compile GLImac :

```
$ cmake ../GLImac && make
```

Run an executable by using this pattern :

```
$ ./GLImac-subdirectory-name/GLImac-subdirectory-name_executable-file-name
```

## Resources

- [OpenGL3+](http://igm.univ-mlv.fr/~biri/OpenGL/opengl.php) - Description of the practicals
