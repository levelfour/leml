#include "type.hh"

LemlType* const typeUnit  = new LemlType({Unit, nullptr, {}});
LemlType* const typeBool  = new LemlType({Bool, nullptr, {}});
LemlType* const typeInt   = new LemlType({Int, nullptr, {}});
LemlType* const typeFloat = new LemlType({Float, nullptr, {}});

LemlType* newty() {
	return new LemlType({Var, nullptr, {}});
}

LemlType::operator std::string() const {
	switch(tag) {
		case Unit:  return "unit";
		case Bool:  return "bool";
		case Int:   return "int";
		case Float: return "float";
		case Fun:
		{
			std::string tystr = "";
			for(auto ty: array) {
				tystr += (static_cast<std::string>(*ty) + std::string(" -> "));
			}
			tystr += static_cast<std::string>(*data);
			return tystr;
		}
		case Tuple:
		{
			std::string tystr = "(";
			for(unsigned long i = 0; i < array.size(); i++) {
				auto ty = array[i];
				tystr += static_cast<std::string>(*ty);
				if(i < array.size() - 1) {
					tystr += ", ";
				} else {
					tystr += ")";
				}
			}
			return tystr;
		}
		case Array:
			return static_cast<std::string>(*data) + " array";
		case Var:
			return "var";
	}
}
