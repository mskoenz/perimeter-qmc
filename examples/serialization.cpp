// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    05.06.2013 16:28:19 EDT
// File:    serialization.cpp

#include <addons/serialize/fserial.hpp>
#include <iostream>
#include <fstream>
#include <accum_simple.hpp>
#include <accum_double.hpp>
#include <bash_parameter3_msk.hpp>
#include <bitset>

int main(int argc, char* argv[])
{
    addon::parameter.set("H", 2);
    addon::parameter.set("file", "bla");
    
    
    addon::parameter.read(argc, argv);
    
    addon::fserial_class ar("arch.txt");
    ar << addon::parameter;
    
    ar >> addon::parameter;
    
    std::cout << addon::parameter["H"] << std::endl;
    std::cout << addon::parameter["file"] << std::endl;
    
    return 0;
}
