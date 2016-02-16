#ifndef __LEML_HH__
#define __LEML_HH__

#ifdef LEML_DEBUG
#define cdbg cerr << "DEBUG: "
#endif

#include <string>
#include <typeinfo>

extern std::string gFilename;
extern bool gVerbose;
extern bool gNostdlib;
extern bool gMem2reg;
extern bool gPartialApp;

#endif // __LEML_HH__
