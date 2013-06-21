// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    20.06.2013 20:25:02 EDT
// File:    vector_support.hpp

#ifndef __VECTOR_SUPPORT_HEADER
#define __VECTOR_SUPPORT_HEADER

#include <iostream>
#include <vector>

//------------------- print & serialize -------------------
template<typename T, typename S>
S & operator<<(S & os, std::vector<T> const & arg) {
    typedef typename std::vector<T>::size_type size_type;
    os << "[";
    for(size_type i = 0; i < arg.size(); ++i) {
        os << arg[i];
        if(i != arg.size() - 1)
            os << ", ";
    }
    os << "]";
    return os;
}
namespace addon {
    template<typename T, typename Archive>
    void serialize(Archive & ar, std::vector<T> & arg) {
        typedef typename ustd::vector<T>::size_type size_type;
        size_type size_ = arg.size();
        ar & size_;
        if(Archive::type == archive_enum::input) {
            arg.resize(size_);
        }
        for(size_type i = 0; i < size_; ++i) {
            ar & arg[i];
        }
    }
}//end namespace addon
#endif //__VECTOR_SUPPORT_HEADER
