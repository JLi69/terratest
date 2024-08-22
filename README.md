# TerraTest

**NOTE: as of August 22nd, 2024 I am archiving this repository and ceasing 
development. I have not worked on this game in a long time and I feel that there
are too many issues that have made this game unmaintainable and I feel
that this project no longer represents my best work. You can read more
about my thoughts in a [blog post](https://jli69.github.io/blog/2024/07/23/criticizing-terratest.html)
I made. I might make a rewrite in the future but that will take a while.**

This is a Terraria clone that I wrote as a programming challenge.

A lot of the code is messy and rather poorly organized and the 
performance/resource usage of the game isn't good at times.

## Libraries Used
[GLFW](https://github.com/glfw/glfw)

[glad](https://github.com/Dav1dde/glad)

[SOIL2](https://github.com/SpartanJ/SOIL2)

## Build
`
git clone --recursive https://github.com/JLi69/terratest
`

(make sure that you have the dependencies to the libraries used
installed on your system)

### Linux:

Tested with gcc but should build with other compilers.

```
cd terratest
mkdir build
cd build
cmake ..
make
```

### Windows:
I built this with MinGW-w64 but I don't believe that this
will work with MSVC as I used some unix headers in some
parts of the game.

```
cd terratest
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
```

## License
All code (shaders/source code) is under the MIT license.

Assets (images) are public domain.
