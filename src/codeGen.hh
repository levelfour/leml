#ifndef __CODE_GEN_HH__
#define __CODE_GEN_HH__

#include <map>
#include <stack>
#include <string>
#include <cassert>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/PassManager.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRbuilder.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include "type.hh"

class NExpression;

class CodeGenBlock {
public:
	llvm::BasicBlock *block;
	std::map<std::string, llvm::Value*> locals;
};

class CodeGenContext {
	std::stack<CodeGenBlock *> blocks;
	llvm::Function *fnMain;
	llvm::Type* typeRet;

public:
	llvm::Module *module;
	llvm::IRBuilder<> *builder;
	CodeGenContext();
	virtual ~CodeGenContext();

	void generateCode(NExpression& root, LemlType* type, bool verbose = false);
	int runCode(bool verbose = false);
	std::map<std::string, llvm::Value*>& locals();
	llvm::BasicBlock *currentBlock();
	void pushBlock(llvm::BasicBlock *block);
	void popBlock();

	void addCoreFunctions(llvm::Function *fn);
};

#endif // __CODE_GEN_HH__
