#include "syntax.hh"

// stream printer for NExpression
std::ostream &operator<<(std::ostream &os, const NExpression &expr) {
	return expr.print(os);
}

std::ostream& NExpression::print(std::ostream& os) const {
	os << "exp";
	return os;
}

std::ostream& NUnit::print(std::ostream& os) const {
	os << "()";
	return os;
}

std::ostream& NBoolean::print(std::ostream& os) const {
	os << value;
	return os;
}

std::ostream& NInteger::print(std::ostream& os) const {
	os << value;
	return os;
}

std::ostream& NFloat::print(std::ostream& os) const {
	os << value;
	return os;
}

std::ostream& NIdentifier::print(std::ostream& os) const {
	os << name;
	return os;
}

std::ostream& NUnaryExpression::print(std::ostream& os) const {
	os << op << "("; expr.print(os); os << ")";
	return os;
}

std::ostream& NBinaryExpression::print(std::ostream& os) const {
	os << op << "("; lhs.print(os); os << ","; rhs.print(os); os << ")";
	return os;
}

std::ostream& NCompExpression::print(std::ostream& os) const {
	os << op << "("; lhs.print(os); os << ","; rhs.print(os); os << ")";
	return os;
}

std::ostream& NIfExpression::print(std::ostream& os) const {
	os << "if ("; cond.print(os); os << ",";
	true_exp.print(os); os << ","; false_exp.print(os); os << ")";
	return os;
}

std::ostream& NLetExpression::print(std::ostream& os) const {
	if(assign != nullptr) {
		os << "let " << id;
		os << " = " << *assign << " in " << std::endl;
		os << *eval;
	} else {
		os << id;
	}
	return os;
}

std::ostream& NFundefExpression::print(std::ostream& os) const {
	os << id.name;
	for(auto arg: args) {
		os << " " << arg->id.name;
	}

	return os;
}

std::ostream& NLetRecExpression::print(std::ostream& os) const {
	os << "let rec " << proto << 
		  " = \n" << body << std::endl << "in " << eval;
	return os;
}

std::ostream& NCallExpression::print(std::ostream& os) const {
	os << "(" << fun << ")";
	for(auto arg: *args) {
		os << " " << *arg;
	}
	return os;
}

std::ostream& NArrayExpression::print(std::ostream& os) const {
	os << "array{size: " << length << ", val: " << data << "}";
	return os;
}

std::ostream& NArrayGetExpression::print(std::ostream& os) const {
	os << array << ".(" << index << ")";
	return os;
}

std::ostream& NArrayPutExpression::print(std::ostream& os) const {
	os << array << ".(" << index << ") <- " << exp;
	return os;
}

std::ostream& NTupleExpression::print(std::ostream& os) const {
	os << "(";
	for(unsigned long i = 0; i < elems.size(); i++) {
		os << *elems[i] << ((i < elems.size() - 1) ? ", " : ")");
	}
	return os;
}

std::ostream& NLetTupleExpression::print(std::ostream& os) const {
	os << "let (";
	for(unsigned long i = 0; i < ids.size(); i++) {
		os << *ids[i] << ((i < ids.size() - 1) ? ", " : ")");
	}
	os << " = " << exp << " in " << std::endl;
	os << eval;
	return os;
}
