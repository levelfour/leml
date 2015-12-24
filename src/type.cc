#include "type.hh"

LemlType* const typeUnit  = new LemlType({Unit, nullptr, {}});
LemlType* const typeBool  = new LemlType({Bool, nullptr, {}});
LemlType* const typeInt   = new LemlType({Int, nullptr, {}});
LemlType* const typeFloat = new LemlType({Float, nullptr, {}});

LemlType* newty() {
	return new LemlType({Var, nullptr, {}});
}
