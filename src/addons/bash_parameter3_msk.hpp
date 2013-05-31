// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    21.04.2012 13:19:09 CEST
// File:    bash_parameter_msk.hpp

#ifndef __BASH_PARAMETER_HEADER_MSK
#define __BASH_PARAMETER_HEADER_MSK

/* minimal code

#include <bash_parameter_msk.hpp>
#include "bash_parameter_msk.hpp"
//parameter is defined in the header
addon::parameter.set(energy, "energy");
addon::parameter.set(magn, "magn", 100); //set with default
addon::parameter.set("equilib"); //set iternal
addon::parameter.read(argc, argv);
addon::parameter.get();

*/

#include <map>
#include <vector>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <typeinfo>
#include <stdexcept>

//timer2_msk.hpp documents addon
namespace addon
{
    namespace detail {
        //------------------- runtime checks -------------------
        template<typename T>
        bool convertable_to(const std::string& str) {
            std::istringstream iss(str);
            T obj;
            iss >> std::ws >> obj >> std::ws;

            if(!iss.eof())
                return false;

            return true; 
        }

        template< typename T >
        inline T convert(const std::string& str)
        {
            std::istringstream iss(str);
            T obj;

            iss >> std::ws >> obj >> std::ws;

            if(!iss.eof())
                throw std::runtime_error("not convertable!");

            return obj; 
        }

        //------------------- compiletime checks -------------------
        template<typename T, typename U>
        struct is_convertable {
            static char check(U const &);
            static double check(...);
            static T t;
            enum{value = (sizeof(char) == sizeof(check(t)))};
        };
        //------------------- chameleon type -------------------
        class chameleon_type {
            typedef std::string type_type;
            
            template<typename T>
            static type_type tn() {
                return type_type(typeid(T).name());
            }
            template<typename T, typename U, bool convertable>
            struct init {
                init<T, U, convertable>(T const & t, U & backup, chameleon_type * b) {
                    backup = U(t);
                    b->v = &backup;
                    b->type = tn<U>();
                }
            };
            template<typename T, typename U>
            struct init<T, U, false> {
                init<T, U, false>(T const & t, U & backup, chameleon_type * b) {
                }
            };
            template<typename T>
            void set(T const & t) {
                v = NULL;
                init<T, double, is_convertable<T, double>::value>(t, dbu, this);
                init<T, std::string, is_convertable<T, std::string>::value>(t, sbu, this);
                if(v == NULL) {
                    std::stringstream ss;
                    ss << "cannot convert type " << typeid(T).name() << " to double or std::string";
                    throw std::runtime_error(ss.str());
                }
            }
        public:
            template<typename T>
            chameleon_type(T const & t) {
                set(t);
            }
            chameleon_type(): v(&dbu), type(tn<double>()), dbu(0) {
            }
            chameleon_type(chameleon_type const & c): v(c.v), type(c.type), dbu(c.dbu), sbu(c.sbu) {
            }
            template<typename T>
            void operator=(T const & t) {
                set(t);
            }
            operator std::string() const {
                if(tn<std::string>() != type){
                    //~ DEBUG_VAR(sbu)
                    //~ DEBUG_VAR(dbu)
                    //~ DEBUG_VAR(type)
                    throw std::runtime_error("wrong conversion to std::string");
                }
                return *(std::string const *)(v);
            }
            operator double() const {
                if(tn<double>() != type) {
                    //~ DEBUG_VAR(sbu)
                    //~ DEBUG_VAR(dbu)
                    //~ DEBUG_VAR(type)
                    throw std::runtime_error("wrong conversion to double");
                }
                return *(double const *)(v);
            }
            void print(std::ostream & os = std::cout) const {
                if(type == tn<double>())
                    os << dbu;
                else if(type == tn<std::string>())
                    os << sbu;
                else
                    os<< "chameleon is empty";
            };
        private:
            void const * v;
            type_type type;
            double dbu;
            std::string sbu;
        };
        std::ostream & operator<<(std::ostream & os, chameleon_type const & b) {
            b.print(os);
            return os;
        }

    }//end namespace detail
    ///  \brief handles argv and assigns the arguments automatically
    ///  
    ///  takes references (or not) and the name for the parameter. upon read it assigns the the incomming argv correctly.
    ///  there is an instance called parameter already created, so no need to construct one in your code
    class bash_parameter_class {
    public:
        typedef detail::chameleon_type store_type;
        typedef std::map<std::string, store_type> map_type;
        ///  \brief the only constructor
        ///  
        bash_parameter_class() {}
        ///  \brief internal variable
        ///  
        ///  @param name is the wanted name for bash. name = "energy" -> bash option = "-energy"
        ///  
        ///  internal variable that is changed on read, if option is given, otherwise 0
        void set(std::string name) {
            set(name, 0);
        }
        ///  \brief internal variable with default
        ///  
        ///  @param name is the wanted name for bash. name = "energy" -> bash option = "-energy"
        ///  @param def is the default value
        ///  
        ///  the internal variable is set to def and changed on read, if option is given
        void set(std::string name, store_type const & val) {
            dict[name] = val;
        }
        ///  \brief reads the argv
        ///  
        ///  looks for options that correspond to those set earier. if found, it changes the variable to the value
        ///  after the option
        void read(int argc, char* argv[]) {
            for(int i = 1; i < argc; i += 2) {
                for(auto it = dict.begin(); it != dict.end(); ++it) {
                    if(("-" + (it->first)) == argv[i]) {
                        if(detail::convertable_to<double>(argv[i+1]))
                            it->second = detail::convert<double>(argv[i+1]);
                        else
                            it->second = detail::convert<std::string>(argv[i+1]);
                    }
                }
            }
        }
        ///  \brief returns the dict
        map_type get() const {
            return dict;
        }
        ///  \brief returns the value to key
        ///  
        ///  just an abreviation for parameter.get()["key"]
        store_type & operator[](std::string const & key) {
            if(dict.find(key) != dict.end())
                return dict[key];
            else
                throw std::runtime_error("key not found in bash_parameter");
        }
    private:
        map_type dict;    ///< the dictionary
    } parameter;
}//end namespace addon

#endif //__BASH_PARAMETER_HEADER_MSK
