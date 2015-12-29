#ifndef __CODE_GEN_HH__
#define __CODE_GEN_HH__

#include <map>
#include <stack>
#include <string>
#include <cassert>
#include <llvm/PassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/IRbuilder.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Analysis/Passes.h>

#include "type.hh"

class NExpression;

class CodeGenBlock {
public:
	llvm::BasicBlock *block;
	std::map<std::string, llvm::Value*> locals;
	CodeGenBlock(llvm::BasicBlock* block): block(block) {}
};

class CodeGenContext {
	std::stack<std::unique_ptr<CodeGenBlock>> blocks;
	llvm::Function* fnMain;
	llvm::Type* typeRet;

public:
	std::unique_ptr<llvm::Module> module;
	std::unique_ptr<llvm::IRBuilder<>> builder;
	std::unique_ptr<llvm::FunctionPassManager> fpm;
	CodeGenContext();
	virtual ~CodeGenContext();

	void generateCode(NExpression& root, std::unique_ptr<LemlType> type, bool verbose = false);
	int runCode(bool verbose = false);
	std::map<std::string, llvm::Value*>& locals();
	llvm::BasicBlock *currentBlock();
	void pushBlock(llvm::BasicBlock *block);
	void popBlock();

	void addCoreFunctions(llvm::Function *fn);
};

#endif // __CODE_GEN_HH__
