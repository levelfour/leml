#ifndef __SYNTAX_HPP__
#define __SYNTAX_HPP__

#include <iostream>
#include <vector>
#include <string>
#include <llvm/IR/Value.h>
#include "type.hh"

enum Operation {
	LNot,
	LNeg,
	LAdd,
	LSub,
	LMul,
	LDiv,
	LFNeg,
	LFAdd,
	LFSub,
	LFMul,
	LFDiv,
	LEq,
	LNeq,
	LLE,
	LLT,
	LGE,
	LGT,
};

class CodeGenContext;
class NExpression;
class NLetExpression;

std::ostream& operator<<(std::ostream& os, const NExpression& expr);

class Node {
public:
	virtual ~Node() {}
	virtual llvm::Value* codeGen(CodeGenContext& context) { return NULL; }
};

class NExpression: public Node {
public:
	virtual llvm::Value* codeGen(CodeGenContext& context) { return nullptr; }
	virtual std::ostream& print(std::ostream& os) const {
		os << "exp";
		return os;
	}
};

class NUnit: public NExpression {};

class NBoolean: public NExpression {
public:
	bool value;
	NBoolean(bool val): value(val) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const {
		os << value;
		return os;
	}
};

class NInteger: public NExpression {
public:
	int value;
	NInteger(int val): value(val) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const {
		os << value;
		return os;
	}
};

class NFloat: public NExpression {
public:
	double value;
	NFloat(double val): value(val) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const {
		os << value;
		return os;
	}
};

class NIdentifier: public NExpression {
public:
	std::string name;
	NIdentifier(std::string s): name(s) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const {
		os << name;
		return os;
	}
};

class NUnaryExpression: public NExpression {
public:
	int op;
	NExpression& expr;
	NUnaryExpression(int op, NExpression &expr): op(op), expr(expr) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const {
		os << op << "("; expr.print(os); os << ")";
		return os;
	}
};

class NBinaryExpression: public NExpression {
public:
	int op;
	NExpression& lhs;
	NExpression& rhs;
	NBinaryExpression(int op, NExpression &expr1, NExpression &expr2): op(op), lhs(expr1), rhs(expr2) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const {
		os << op << "("; lhs.print(os); os << ","; rhs.print(os); os << ")";
		return os;
	}
};

class NCompExpression: public NExpression {
public:
	int op;
	NExpression& lhs;
	NExpression& rhs;
	NCompExpression(int op, NExpression &expr1, NExpression &expr2): op(op), lhs(expr1), rhs(expr2) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const {
		os << op << "("; lhs.print(os); os << ","; rhs.print(os); os << ")";
		return os;
	}
};

class NIfExpression: public NExpression {
public:
	NExpression& cond;
	NExpression& true_exp;
	NExpression& false_exp;
	NIfExpression(NExpression &cond, NExpression &expr1, NExpression &expr2):
		cond(cond), true_exp(expr1), false_exp(expr2) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const {
		os << "if ("; cond.print(os); os << ",";
		true_exp.print(os); os << ","; false_exp.print(os); os << ")";
		return os;
	}
};

class NLetExpression: public NExpression {
public:
	NIdentifier& id;
	LemlType* t;
	NExpression* assign;
	NExpression* eval;
	NLetExpression(NIdentifier& s):
		id(s), t(newty()), assign(nullptr), eval(nullptr) {}
	NLetExpression(NIdentifier& s, NExpression* assign, NExpression* eval):
		id(s), t(newty()), assign(assign), eval(eval) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const {
		os << "let " << id;
		if(assign != nullptr) {
			os << " = " << *assign;
		}
		os << std::endl;
		return os;
	}
};

class NFundefExpression: public NExpression {
public:
	NIdentifier& id;
	std::vector<NLetExpression*> args;
	NExpression& block;
	NFundefExpression(NIdentifier& id, std::vector<NLetExpression*> args, NExpression& block):
		id(id), args(args), block(block) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const {
		os << block;
		return os;
	}
};

class NLetRecExpression: public NExpression {
public:
	NIdentifier& id;
	LemlType* t;
	std::vector<NLetExpression*> args;
	NExpression& body;
	NExpression& eval;
	NLetRecExpression(NIdentifier& id, std::vector<NLetExpression*> args, NExpression& body, NExpression& expr):
		id(id), t(newty()), args(args), body(body), eval(expr) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const {
		os << "let rec " << id.name;
		for(auto arg: args) {
			os << " " << arg->id.name;
		}
		os << " = \n" << body << std::endl << "in " << eval;
		return os;
	}
};

class NCallExpression: public NExpression {
public:
	NExpression& fun;
	std::vector<NExpression*> *args;
	NCallExpression(NExpression& fun, std::vector<NExpression*> *args):
		fun(fun), args(args) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const {
		os << "(" << fun << ")";
		for(auto arg: *args) {
			os << " " << *arg;
		}
		return os;
	}
};

class NArrayExpression: public NExpression {
public:
	NExpression& length;
	NExpression& data;
	std::vector<NExpression*> *array;
	NArrayExpression(NExpression& length, NExpression& data): length(length), data(data) {
		array = new std::vector<NExpression*>;
	}
	virtual ~NArrayExpression() { delete array; }
	virtual llvm::Value* codeGen(CodeGenContext& context);
};

#endif // __SYNTAX_HPP__
