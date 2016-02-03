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
#include <llvm/IRReader/IRReader.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/SourceMgr.h>
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
	union composite {
		int d;
		float f;
	};

	std::string builtinIRFileName = "__builtins.ll";
	std::stack<std::unique_ptr<CodeGenBlock>> blocks;
	llvm::Function* fnMain;
	llvm::Type* typeRet;
	composite resultValue;

public:
	std::unique_ptr<llvm::Module> module;
	std::unique_ptr<llvm::IRBuilder<>> builder;
	std::unique_ptr<llvm::FunctionPassManager> fpm;
	CodeGenContext();
	virtual ~CodeGenContext();

	void setEnv(TypeEnv env);
	void setBuiltInIR(std::string filename);
	bool generateCode(NExpression& root, std::unique_ptr<LemlType> type);
	void runCode();
	int getIntResult();
	float getFloatResult();
	std::map<std::string, llvm::Value*>& locals();
	llvm::BasicBlock *currentBlock();
	void pushBlock(llvm::BasicBlock *block);
	void popBlock();
	bool linkModule(llvm::Module *dest, std::string filename);
};

#endif // __CODE_GEN_HH__
