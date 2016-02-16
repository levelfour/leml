#ifndef __ERROR_HH_
#define __ERROR_HH_

#include <exception>
#include "type.hh"

class VectorLengthError: public std::invalid_argument {
public:
	VectorLengthError(): std::invalid_argument("iteration over sequences of different length") {}
};

template<class T1, class T2>
static void vectorLengthAssertion(std::vector<T1> v1, std::vector<T2> v2) {
	if(v1.size() != v2.size()) {
		throw VectorLengthError();
	}
}

#endif // __ERROR_HH_
