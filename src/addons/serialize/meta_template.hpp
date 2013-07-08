// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    13.06.2013 18:45:18 EDT
// File:    meta_template.hpp

#ifndef __META_TEMPLATE_HEADER
#define __META_TEMPLATE_HEADER

#include <type_traits>

//------------------- mtp stuff -------------------
template<typename T>
struct is_const {
    enum{value = 0};
};
template<typename T>
struct is_const<T const> {
    enum{value = 1};
};

template<typename T>
struct is_class {
    template<typename U> static char check(void(U::*)(void));
    template<typename U> static double check(...);
    
    enum { value = (sizeof(char) == sizeof(check<T>(0))) };
};

template<typename T>
struct is_array {
    //~ static T t;
    //~ 
    //~ template<int N> struct wrap {typedef char type;};
    //~ template<typename U> static U * ptr(U u[]);
    //~ template<typename U> static U ptr(U u);
    //~ 
    //~ template<typename U> static typename wrap<sizeof(t = ptr(t))>::type check(int);
    //~ template<typename U> static double check(...);
    
    //~ enum{value = (sizeof(double) == sizeof(check<T>(int())))};
    enum{value = std::is_array<T>::value};
};
//~ template<typename T> //remove constness before doing array checks since const i = j is also illegal
//~ struct is_array<T const> {
    //~ enum{value = is_array<T>::value};
//~ };

#endif //__META_TEMPLATE_HEADER
