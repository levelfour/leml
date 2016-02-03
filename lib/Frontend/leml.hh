#ifndef __LEML_HH__
#define __LEML_HH__

#ifdef LEML_DEBUG
#define cdbg cerr
#endif

#include <string>

extern std::string gFilename;
extern bool gVerbose;
extern bool gNostdlib;
extern bool gMem2reg;

#endif // __LEML_HH__
