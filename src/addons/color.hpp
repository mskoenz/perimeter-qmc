// Author:  Mario S. Könz <mskoenz@gmx.net>
// Date:    25.04.2013 18:09:04 EDT
// File:    color.hpp

#ifndef __COLOR_HEADER
#define __COLOR_HEADER

#ifndef NO_COLOR

#define CLRSCR "\033[2J\033[100A"
#define BLACK "\033[0;30m"
#define BLACKB "\033[1;30m"
#define RED "\033[0;31m"
#define REDB "\033[1;31m"
#define GREEN "\033[0;32m"
#define GREENB "\033[1;32m"
#define YELLOW "\033[0;33m"
#define YELLOWB "\033[1;33m"
#define BLUE "\033[0;34m"
#define BLUEB "\033[1;34m"
#define MAGENTA "\033[0;35m"
#define MAGENTAB "\033[1;35m"
#define CYAN "\033[0;36m"
#define CYANB "\033[1;36m"
#define WHITE "\033[0;37m"
#define WHITEB "\033[1;37m"

#define BLACKBG "\033[0;40m"
#define REDBG "\033[0;41m"
#define GREENBG "\033[0;42m"
#define YELLOWBG "\033[0;43m"
#define BLUEBG "\033[0;44m"
#define MAGENTABG "\033[0;45m"
#define CYANBG "\033[0;46m"
#define WHITEBG "\033[0;47m"

#define NONE "\033[0m" 

#else //NO_COLOR

#define CLRSCR ""

#define BLACK ""
#define BLACKB ""
#define RED ""
#define REDB ""
#define GREEN ""
#define GREENB ""
#define YELLOW ""
#define YELLOWB ""
#define BLUE ""
#define BLUEB ""
#define MAGENTA ""
#define MAGENTAB ""
#define CYAN ""
#define CYANB ""
#define WHITE ""
#define WHITEB ""

#define BLACKBG ""
#define REDBG ""
#define GREENBG ""
#define YELLOWBG ""
#define BLUEBG ""
#define MAGENTABG ""
#define CYANBG ""
#define WHITEBG ""

#define NONE "" 

#endif //NO_COLOR

#endif //__COLOR_HEADER
