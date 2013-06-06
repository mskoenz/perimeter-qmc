// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    05.06.2013 16:28:19 EDT
// File:    serialization.cpp

#include <serialize.hpp>
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
    
    std::bitset<6> a("10100");
    
    std::ofstream ofs;
    ofs.open("arch.txt");
    //~ a.serialize(ofs);
    addon::stream(ofs, a);
    addon::parameter.serialize(ofs);
    ofs.close();
    
    std::ifstream ifs;
    ifs.open("arch.txt");
    addon::stream(ifs, a);
    
    addon::parameter.serialize(ifs);
    ifs.close();
    
    std::cout << addon::parameter["H"] << std::endl;
    std::cout << addon::parameter["file"] << std::endl;
    
    return 0;
}
