#ifndef __INFER_HH__
#define __INFER_HH__

#include <vector>
#include <map>
#include <exception>
#include <llvm/IR/Type.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>

#include "syntax.hh"
#include "type.hh"

typedef std::map<std::string, LemlType*> TypeEnv;

static bool occur(LemlType* r1, LemlType* t);
void unify(LemlType* t1, LemlType* t2);
LemlType* infer(NExpression* expr, TypeEnv env);
static llvm::Type* llvmType(LemlType* type);

class UnificationError: public std::invalid_argument {
public:
	UnificationError(): std::invalid_argument("unification error") {}
	UnificationError(LemlType* t1, LemlType* t2):
		std::invalid_argument("unification error: "
				+ static_cast<std::string>(*t1) + " and "
				+ static_cast<std::string>(*t2)) {}
};

#endif // __INFER_HH__
