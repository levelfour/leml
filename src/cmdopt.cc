#include <cstring>
#include <fstream>
#include "cmdopt.hh"

void OptionParser::build() {
	for(int i = 1; i < argc; i++) {
		if(argv[i][0] == '-') {
			if(std::strlen(argv[i]) < 2) throw std::invalid_argument("-");

			std::string s;

			if(argv[i][1] == '=') {
				if(std::strlen(argv[i]) < 3) throw std::invalid_argument("--");
				s = argv[i] + 2;
			} else {
				s = argv[i] + 1;
			}

			if(spec.find(s) == spec.end()) throw std::invalid_argument(argv[i]);

			if(spec[s] == 0) {
				// boolean option
				opt[s] = "true";
			} else if(spec[s] == 1) {
				// string option
				opt[s] = argv[++i];
			}
		} else {
			if(std::ifstream(argv[i])) {
				opt["default"] = argv[i];
			} else {
				throw FileDoesNotExist(argv[i]);
			}
		}
	}
}

void OptionParser::set(std::map<std::string, int> spec) {
	this->spec = spec;
}

std::string OptionParser::get(std::string s) {
	try {
		return opt[s];
	} catch(std::exception _) {
		return "";
	}
}
