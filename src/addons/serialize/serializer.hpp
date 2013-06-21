// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    29.05.2013 23:04:33 EDT
// File:    serializer.hpp

#ifndef __SERIALIZER_HEADER
#define __SERIALIZER_HEADER

//~ #define __TYPE_SIZE_CHECK
#define __LESS_ZERO

#include "archive_enum.hpp"
#include "meta_template.hpp"
#include "byte_operation.hpp"
#include "../color.hpp"

#include <assert.h>

namespace addon {
    //------------------- checks of T is a class and has a template method called serialize -------------------
    template<typename T, bool is_a_class> 
    struct has_serialize_impl {
        struct archive_proto_type {
            static archive_enum const type = archive_enum::undef;
            template<typename U> void operator &(U & u);
        };
        template<void(T::*)(archive_proto_type &)> struct helper {typedef char type;};
        
        template<typename U> static char check(typename helper<&U::template serialize<archive_proto_type>>::type);
        template<typename U> static double check(...);
        
        enum { value = (sizeof(char) == sizeof(check<T>(0))) };
    };
    template<typename T> 
    struct has_serialize_impl<T, false> { //if T is not a class it cannot have serialize
        enum { value = false };
    };
    template<typename T> 
    struct has_serialize {
        enum { value = has_serialize_impl<T, is_class<T>::value>::value };
    };
    //------------------- implementation -------------------
    template<typename T, typename Archive>
    void serialize(Archive & ar, T & t);
    //------------------- recursively serializes arrays :D -------------------
    template<typename T, typename Archive, archive_enum type, bool is_an_array> //default == true
    struct impl_version {
        static void get(T & t, Archive & ar) {
            for(uint16_t i = 0; i < sizeof(t)/sizeof(t[0]); ++i) {
                serialize(ar, t[i]); //<-calls serialize for subarrays
            }
        }
    };
    template<typename T, typename Archive>
    struct impl_version<T, Archive, archive_enum::output, false> { //if no serialize, then byte-serialize
        static void get(T & t, Archive & ar) {
            #ifdef __TYPE_SIZE_CHECK
                ar.write(sizeof(t));
            #endif //__TYPE_SIZE_CHECK
            #ifdef __LESS_ZERO
                uint8_t idx = sizeof(t);
                while(idx != 0) {
                    if(util::read_byte(t, idx - 1) != 0)
                        break;
                    else
                        --idx;
                }
                ar.write(idx);
                for(uint8_t i = 0; i < idx; ++i)
                    ar.write(util::read_byte(t, i));
            #else //__LESS_ZERO
                for(uint8_t i = 0; i < sizeof(t); ++i)
                    ar.write(util::read_byte(t, i));
            #endif //__LESS_ZERO
        }
    };
    template<typename T, typename Archive>
    struct impl_version<T, Archive, archive_enum::input, false> {
        static void get(T & t, Archive & ar) {
            #ifdef __TYPE_SIZE_CHECK
                uint8_t exp_size = ar.read();
                assert(sizeof(t) == exp_size);
            #endif //__TYPE_SIZE_CHECK
            #ifdef __LESS_ZERO
                uint8_t idx = ar.read();
                for(uint8_t i = 0; i < idx; ++i)
                    util::write_byte(t, i, ar.read());
                for(uint8_t i = idx; i < sizeof(t); ++i) {
                    util::write_byte(t, i, 0x00);
                }
            #else //__LESS_ZERO
                for(uint8_t i = 0; i < sizeof(t); ++i)
                    util::write_byte(t, i, ar.read());
            #endif //__LESS_ZERO
        }
    };

    template<typename T, bool has_serialize_tpl, typename Archive> //default == true
    struct serialize_helper {
        static void get(Archive & ar, T & t) {
            t.serialize(ar);
        }
    };

    template<typename T, typename Archive> //has no serialize_tpl
    struct serialize_helper<T, false, Archive> {
        static void get(Archive & ar, T & t) {
            impl_version<T, Archive, Archive::type, is_array<T>::value>().get(t, ar);
        }
    };

    template<typename T, typename Archive>
    void serialize(Archive & ar, T & t) {
        serialize_helper<T, has_serialize<T>::value, Archive>().get(ar, t);
    }
    //------------------- of class -------------------
    template<typename D>
    class oss_class {
        typedef uint16_t size_type;
    public:
        static archive_enum const type = archive_enum::output;
        
        oss_class(D & data): data(data) {
        }
        template<typename T>
        oss_class & operator&(T & t) {
            serialize(*this, t);
            return (*this);
        }
        void write(uint8_t const & in) {
            data.write((char*)&in, 1);
        }
    private:
        D & data;
    };
    //------------------- if class -------------------
    template<typename D>
    class iss_class {
        typedef uint16_t size_type;
    public:
        static archive_enum const type = archive_enum::input;
        
        iss_class(D & data): data(data) {
        }
        template<typename T>
        iss_class & operator&(T & t) {
            serialize(*this, t);
            return (*this);
        }
        uint8_t read() {
            char res = 0;
            data.read(&res, 1);
            return res;
        }
    private:
        D & data;
    };
}//end namespace addon
#endif //__SERIALIZER_HEADER
