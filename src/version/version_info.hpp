// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    16.07.2012 10:58:51 CEST
// File:    version_info.hpp

#ifndef __VERSION_INFO_HEADER
#define __VERSION_INFO_HEADER

#include <version/config.hpp>
#include <version/build.hpp>

#include <iostream>

void version_info()
{
    std::cout << "This is Version: " << MAJOR << "." << MINOR << " Build: " << BUILD_NR << std::endl;
}


#endif //__VERSION_INFO_HEADER
