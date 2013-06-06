// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    05.06.2013 13:34:39 EDT
// File:    demo_header.hpp
/*
#ifndef __DEMO_HEADER_HEADER
#define __DEMO_HEADER_HEADER

template<typename T>
class test_class {
public:
    test_class();
    test_class(T const &);
    T const &  get() const;
    operator bool() const;
private:
    T a;
};

extern int ext;

#endif //__DEMO_HEADER_HEADER

// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    12.04.2013 17:15:01 EDT
// File:    demo.cpp

#include <demo_header.hpp>

template<typename T>
test_class<T>::test_class(): a(0) {
}
    
template<typename T>
test_class<T>::test_class(T const & in): a(in+ext) {
}
template<typename T>
T const & test_class<T>::get() const {
        return a;
}
template<typename T>
test_class<T>::operator bool() const {
        return (a == T());
}

template class test_class<int>;
template class test_class<double>;
*/
