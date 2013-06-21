// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    20.06.2013 11:29:38 EDT
// File:    fserial.hpp

#ifndef __FSERIAL_HEADER
#define __FSERIAL_HEADER


#include "serializer.hpp"
#include "support/map_support.hpp"
#include "support/string_support.hpp"
#include "support/vector_support.hpp"
#include "support/bitset_support.hpp"
#include "support/boost_multiarray_support.hpp"

#include <fstream>

namespace addon {
        
    class fserial_class {
    public:
        //------------------- ctors -------------------
        fserial_class(): name_("default.bin"), oss_(ofs_), iss_(ifs_) {
        }
        fserial_class(std::string const & name): name_(name), oss_(ofs_), iss_(ifs_) {
        }
        //------------------- ops -------------------
        void set_name(std::string const & name) {
            name_ = name;
        }
        template<typename T>
        fserial_class & operator<<(T & t) {
            ofs_.open(name_, std::ios::out | std::ios::binary);
            oss_ & t;
            ofs_.close();
            return (*this);
        }
        template<typename T>
        fserial_class & operator>>(T & t) {
            ifs_.open(name_, std::ios::in | std::ios::binary);
            iss_ & t;
            ifs_.close();
            return (*this);
        }
    private:
        std::string name_;
        std::ofstream ofs_;
        std::ifstream ifs_;
        addon::oss_class<std::ofstream> oss_;
        addon::iss_class<std::ifstream> iss_;
    };
}//end namespace addon

#endif //__FSERIAL_HEADER
