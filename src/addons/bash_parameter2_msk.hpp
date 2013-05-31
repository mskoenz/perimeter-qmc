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
    ///  \brief handles argv and assigns the arguments automatically
    ///  
    ///  takes references (or not) and the name for the parameter. upon read it assigns the the incomming argv correctly.
    ///  there is an instance called parameter already created, so no need to construct one in your code
    class bash_parameter_class {
    public:
        ///  \brief the only constructor
        ///  
        ///  creates a vector for variable that are ment to be stored inside and have no reference to an outer variable. the maximal length for now is 25
        bash_parameter_class(): inside(25, 0) {}
        ///  \brief external variable
        ///  
        ///  @param val is the reference to the variable
        ///  @param name is the wanted name for bash. name = "energy" -> bash option = "-energy"
        ///  
        ///  set with an external variable that is changed on read, if option is given
        void set(double & val, std::string name) {
            std::stringstream os;
            os << "-" << name;
            dict[os.str()] = &val;
        }
        ///  \brief external variable with default
        ///  
        ///  @param val is the reference to the variable
        ///  @param name is the wanted name for bash. name = "energy" -> bash option = "-energy"
        ///  @param def is the default value
        ///  
        ///  val is set to def and changed on read, if option is given
        void set(double & val, std::string name, double const & def) {
            val = def;
            set(val, name);
        }
        ///  \brief internal variable
        ///  
        ///  @param name is the wanted name for bash. name = "energy" -> bash option = "-energy"
        ///  
        ///  internal variable that is changed on read, if option is given, otherwise 0
        void set(std::string name) {
            set(inside[index++], name);
        }
        ///  \brief internal variable with default
        ///  
        ///  @param name is the wanted name for bash. name = "energy" -> bash option = "-energy"
        ///  @param def is the default value
        ///  
        ///  the internal variable is set to def and changed on read, if option is given
        void set(std::string name, double const & def) {
            set(inside[index++], name, def);
        }
        ///  \brief reads the argv
        ///  
        ///  looks for options that correspond to those set earier. if found, it changes the variable to the value
        ///  after the option
        void read(int argc, char* argv[]) {
            for(int i = 1; i < argc; i += 2) {
                for(std::map<std::string, double*>::iterator it = dict.begin(); it != dict.end(); ++it) {
                    if((it->first) == argv[i])
                        *(it->second) = atof(argv[i+1]);
                }
            }
        }
        ///  \brief returns a map with all values
        ///  
        ///  is a copy, no references anymore. contains all set parameters
        std::map<std::string, double> get() {
            std::map<std::string, double> res;
            for(std::map<std::string, double*>::iterator it = dict.begin(); it != dict.end(); ++it) {
                res[it->first] = *(it->second);
            }
            return res;
        }
        ///  \brief returns the value to key
        ///  
        ///  just an abreviation for parameter.get()["key"]
        double operator[](std::string const & key) {
            if(dict.find(key) != dict.end())
                return *dict[key];
            else
                throw std::runtime_error("key not found in bash_parameter");
            return 0;
        }
    private:
        std::map<std::string, double*> dict;    ///< the dictionary that stores the references with the names as keys
        std::vector<double> inside; ///< for inits that don't provide an external variable
        unsigned int index; ///< incremented every time an internal variable is set
    } parameter;
}//end namespace addon

#endif //__BASH_PARAMETER_HEADER_MSK
