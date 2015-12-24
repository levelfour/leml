#ifndef __TYPE_HH__
#define __TYPE_HH__

#include <vector>

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
};

LemlType* newty();

extern LemlType* const typeUnit;
extern LemlType* const typeBool;
extern LemlType* const typeInt;
extern LemlType* const typeFloat;

#endif //__TYPE_HH__
