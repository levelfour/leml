#ifndef __TYPE_HH__
#define __TYPE_HH__

#include <iostream>
#include <string>
#include <vector>
#include <map>

enum LemlTypeTag {
	Unit,
	Bool,
	Int,
	Float,
	Fun,
	Tuple,
	Array,
	Var,
};

// imitation of algebraic data type
struct LemlType {
	// constructor (tag)
	LemlTypeTag tag;

	// data
	LemlType* data;

	// list data for Function type, Tuple type
	std::vector<LemlType*> array;

	operator std::string() const;
	friend std::ostream& operator<<(std::ostream& os, const LemlType& ty) {
		os << static_cast<std::string>(ty);
		return os;
	}
};

LemlType* newty();

extern LemlType* const typeUnit;
extern LemlType* const typeBool;
extern LemlType* const typeInt;
extern LemlType* const typeFloat;

typedef std::map<std::string, LemlType*> TypeEnv;

#endif //__TYPE_HH__
