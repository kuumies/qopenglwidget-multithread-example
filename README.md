# qopenglwidget-multithread-example
A QOpenGLWidget OpenGL multithread example

This is a simple example how to render into QOpenGLWidget's surface from non-UI thread. The rendering result is a basic rotating quad mesh where the shading is done with vertex colors. The example uses OpenGL 3.3 pipeline for rendering. The rendering is done using native OpenGL API, not with Qt OpenGL classes. The matrix and quaternion math is done with GLM library.

Example is based on 'Threaded QOpenGLWidget' example that can be found from the Qt SDK examples.

![Example](screenshot.gif?raw=true "Example")

*Image 1. The screenshot of the example. Note that the image displays colors heavily quantized.*

## Building

This example requires c++11 support from the compiler. It is assumed that Qt 5.4 or later and Cmake 3.0.0 or later are installed.

### OSX

```
# Clone the source
git clone https://github.com/kuumies/qopenglwidget-multithread-example.git
# Create build directory
cd qopenglwidget-multithread-example
mkdir build
cd build
# Run cmake, note that you need to tell cmake were to 
# find the Qt (building from Qt Creator seems to auto-
# matically fix this). Below I have set my Qt install 
# path.
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=/users/kuumies/Qt5.5.1/5.5/clang_64/ -DCMAKE_INSTALL_PREFIX=install ../.
# Build and install. The output binary can be found 
# from build/install/bin directory.
make install
```
Tested with:

```
OS X Yosemite 10.10.5
Clang 64 bit 600.0.51
Qt 5.5.1
Cmake 3.4.1
```
