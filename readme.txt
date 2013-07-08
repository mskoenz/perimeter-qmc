To get this code working you need the following:

1) CMake
2) c++11 conformant c++compiler
3) boost

The following commands will get the code and test if it builds

git clone git@github.com:mskoenz/perimeter-qmc.git
cd perimeter-qmc/
mkdir build
cd build/

Specify what compiler you use in the CMakeLists.txt that is located in the same
folder as this file and change line 5 if you're not using g++-4.7

cmake ../
make sim

If it compiled, your system has the necessary tools

good job!
