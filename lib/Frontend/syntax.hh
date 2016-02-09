#ifndef __SYNTAX_HPP__
#define __SYNTAX_HPP__

#include <iostream>
#include <vector>
#include <string>
#include <llvm/IR/Value.h>
#include "leml.hh"
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

class Node;
class NExpression;
class NUnit;
class NBoolean;
class NInteger;
class NFloat;
class NIdentifier;
class NUnaryExpression;
class NBinaryExpression;
class NCompExpression;
class NIfExpression;
class NLetExpression;
class NFundefExpression;
class NLetRecExpression;
class NArrayExpression;
class NArrayGetExpression;
class NArrayPutExpression;
class NTupleExpression;
class NLetTupleExpression;

std::ostream& operator<<(std::ostream& os, const NExpression& expr);

class Node {
public:
	virtual ~Node() {}
	virtual llvm::Value* codeGen(CodeGenContext& context) { return NULL; }
};

class NExpression: public Node {
public:
	virtual llvm::Value* codeGen(CodeGenContext& context) { return nullptr; }
	virtual std::ostream& print(std::ostream& os) const;
};

class NUnit: public NExpression {
public:
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NBoolean: public NExpression {
public:
	bool value;
	NBoolean(bool val): value(val) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NInteger: public NExpression {
public:
	int value;
	NInteger(int val): value(val) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NFloat: public NExpression {
public:
	double value;
	NFloat(double val): value(val) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NIdentifier: public NExpression {
public:
	std::string name;
	NIdentifier(std::string s): name(s) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NUnaryExpression: public NExpression {
public:
	int op;
	NExpression& expr;
	NUnaryExpression(int op, NExpression &expr): op(op), expr(expr) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NBinaryExpression: public NExpression {
public:
	int op;
	NExpression& lhs;
	NExpression& rhs;
	NBinaryExpression(int op, NExpression &expr1, NExpression &expr2): op(op), lhs(expr1), rhs(expr2) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NCompExpression: public NExpression {
public:
	int op;
	NExpression& lhs;
	NExpression& rhs;
	LemlType *t;
	NCompExpression(int op, NExpression &expr1, NExpression &expr2): op(op), lhs(expr1), rhs(expr2) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NIfExpression: public NExpression {
public:
	NExpression& cond;
	NExpression& true_exp;
	NExpression& false_exp;
	NIfExpression(NExpression &cond, NExpression &expr1, NExpression &expr2):
		cond(cond), true_exp(expr1), false_exp(expr2) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
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
	virtual std::ostream& print(std::ostream& os) const;
};

class NFundefExpression: public NExpression {
public:
	NIdentifier& id;
	std::vector<NLetExpression*> args;
	LemlType* t;
	NFundefExpression(NIdentifier& id, std::vector<NLetExpression*> args):
		id(id), args(args) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NLetRecExpression: public NExpression {
public:
	NFundefExpression *proto;
	LemlType* t;
	NExpression& body;
	NExpression& eval;
	NLetRecExpression(NFundefExpression *proto, NExpression& body, NExpression& expr):
		proto(proto), t(newty()), body(body), eval(expr) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NCallExpression: public NExpression {
public:
	NExpression& fun;
	std::vector<NExpression*> *args;
	NCallExpression(NExpression& fun, std::vector<NExpression*> *args):
		fun(fun), args(args) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
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
	virtual std::ostream& print(std::ostream& os) const;
};

class NArrayGetExpression: public NExpression {
public:
	NIdentifier& array;
	NInteger& index;
	NArrayGetExpression(NIdentifier& array, NInteger& index): array(array), index(index) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NArrayPutExpression: public NExpression {
public:
	NIdentifier& array;
	NInteger& index;
	NExpression& exp;
	NArrayPutExpression(NIdentifier& array, NInteger& index, NExpression& exp): array(array), index(index), exp(exp) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NTupleExpression: public NExpression {
public:
	std::vector<NExpression*> elems;
	std::vector<LemlType*> types;
	NTupleExpression(std::vector<NExpression*> elems): elems(elems) { types.reserve(elems.size()); }
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

class NLetTupleExpression: public NExpression {
public:
	std::vector<NLetExpression*> ids;
	NExpression& exp;
	NExpression& eval;
	NLetTupleExpression(std::vector<NLetExpression*> ids, NExpression& exp, NExpression& eval): ids(ids), exp(exp), eval(eval) {}
	virtual llvm::Value* codeGen(CodeGenContext& context);
	virtual std::ostream& print(std::ostream& os) const;
};

#endif // __SYNTAX_HPP__
