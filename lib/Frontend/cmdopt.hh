#ifndef __CMDOPT_HH__
#define __CMDOPT_HH__

#include <string>
#include <map>
#include <exception>

class OptionParser {

private:
	std::map<std::string, int> spec;
	std::map<std::string, std::string> opt;
	int argc;
	char** argv;

public:
	OptionParser(int argc, char** argv): argc(argc), argv(argv) {}
	void build();
	void set(std::map<std::string, int> spec);
	std::string get(std::string s);
};

class FileDoesNotExist: public std::invalid_argument {
public:
	FileDoesNotExist(std::string name): std::invalid_argument(name) {}
};

#endif //__CMDOPT_HH__
