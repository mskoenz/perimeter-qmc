

git clone git@github.com:mskoenz/perimeter-qmc.git
cd perimeter-qmc/
mkdir build
cd build/
cmake ../
make sim
cp ../examples/shift.txt examples/shift.txt
./examples/sim
