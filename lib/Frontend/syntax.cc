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
	switch(op) {
		case LNeg:  os << "-";  break;
		case LFNeg: os << "-."; break;
	}
	if( typeid(expr) == typeid(NInteger) ||
		typeid(expr) == typeid(NFloat) ||
		typeid(expr) == typeid(NBoolean) ||
		typeid(expr) == typeid(NUnit) ||
		typeid(expr) == typeid(NIdentifier)) {
		expr.print(os);
	} else {
		os << "("; expr.print(os); os << ")";
	}
	return os;
}

std::ostream& NBinaryExpression::print(std::ostream& os) const {
	if( typeid(lhs) == typeid(NInteger) ||
		typeid(lhs) == typeid(NFloat) ||
		typeid(lhs) == typeid(NBoolean) ||
		typeid(lhs) == typeid(NUnit) ||
		typeid(lhs) == typeid(NIdentifier)) {
		lhs.print(os);
	} else {
		os << "("; lhs.print(os); os << ")";
	}
	switch(op) {
		case LAdd:  os << "+";  break;
		case LSub:  os << "-";  break;
		case LMul:  os << "*";  break;
		case LDiv:  os << "/";  break;
		case LFAdd: os << "+."; break;
		case LFSub: os << "-."; break;
		case LFMul: os << "*."; break;
		case LFDiv: os << "/."; break;
	}
	if( typeid(rhs) == typeid(NInteger) ||
		typeid(rhs) == typeid(NFloat) ||
		typeid(rhs) == typeid(NBoolean) ||
		typeid(rhs) == typeid(NUnit) ||
		typeid(rhs) == typeid(NIdentifier)) {
		rhs.print(os);
	} else {
		os << "("; rhs.print(os); os << ")";
	}
	return os;
}

std::ostream& NCompExpression::print(std::ostream& os) const {
	if( typeid(lhs) == typeid(NInteger) ||
		typeid(lhs) == typeid(NFloat) ||
		typeid(lhs) == typeid(NBoolean) ||
		typeid(lhs) == typeid(NUnit) ||
		typeid(lhs) == typeid(NIdentifier)) {
		lhs.print(os);
	} else {
		os << "("; lhs.print(os); os << ")";
	}
	switch(op) {
		case LEq:  os << "=";  break;
		case LNeq: os << "<>"; break;
		case LLT:  os << "<";  break;
		case LLE:  os << "<="; break;
		case LGT:  os << ">";  break;
		case LGE:  os << ">="; break;
	}
	if( typeid(rhs) == typeid(NInteger) ||
		typeid(rhs) == typeid(NFloat) ||
		typeid(rhs) == typeid(NBoolean) ||
		typeid(rhs) == typeid(NUnit) ||
		typeid(rhs) == typeid(NIdentifier)) {
		rhs.print(os);
	} else {
		os << "("; rhs.print(os); os << ")";
	}
	return os;
}

std::ostream& NIfExpression::print(std::ostream& os) const {
	os << "if "; cond.print(os); os << " then "; true_exp.print(os);
	os << std::endl << "else ";
	false_exp.print(os);
	return os;
}

std::ostream& NLetExpression::print(std::ostream& os) const {
	if(assign != nullptr) {
		os << "let (" << id << ":" << *t;
		os << ") = " << *assign << std::endl << "in ";
		os << *eval;
	} else {
		os << id << ":" << *t;
	}
	return os;
}

std::ostream& NFundefExpression::print(std::ostream& os) const {
	os << id.name;
	for(auto arg: args) {
		os << " (" << arg->id.name << ":" << *arg->t << ")";
	}

	return os;
}

std::ostream& NLetRecExpression::print(std::ostream& os) const {
	os << "let rec " << *proto << 
		  " = \n" << body << std::endl << "in " << eval;
	return os;
}

std::ostream& NCallExpression::print(std::ostream& os) const {
	if(typeid(fun) == typeid(NIdentifier)) {
		os << "(" << fun;
	} else {
		os << "((" << fun << ")";
	}
	for(auto arg: *args) {
		if( typeid(*arg) == typeid(NInteger) ||
			typeid(*arg) == typeid(NFloat) ||
			typeid(*arg) == typeid(NBoolean) ||
			typeid(*arg) == typeid(NUnit) ||
			typeid(*arg) == typeid(NIdentifier)) {
			os << " " << *arg;
		} else {
			os << " (" << *arg << ")";
		}
	}
	os << ")";
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
