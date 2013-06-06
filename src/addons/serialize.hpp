// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    05.06.2013 16:56:06 EDT
// File:    serialize.hpp

#ifndef __SERIALIZE_HEADER
#define __SERIALIZE_HEADER

#include <fstream>
namespace addon {
    
    namespace detail {
        uint64_t stream_count = 0;
    }//end namespace detail
    
    template<typename T>
    void stream(std::ofstream & ofs, T & t) {
        ofs << t << " ";
        ++detail::stream_count;
        if(detail::stream_count > 25) {
            ofs << std::endl;
            detail::stream_count = 0;
        }
    }
    
    template<typename T>
    void stream(std::ifstream & ifs, T & t) {
        ifs >> t;
    }
}//end namespace addon

#endif //__SERIALIZE_HEADER
