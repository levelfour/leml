#include "codeGen.hh"
#include "syntax.hh"
#include "parser.hh"
#include "infer.hh"

/*
 * the definition of CodeGenContext class
 */

CodeGenContext::CodeGenContext() {
	// create llvm::Module for each program source
	// TODO: replace "main" with source code name
	module = new llvm::Module("main", llvm::getGlobalContext());

	builder = new llvm::IRBuilder<>(llvm::getGlobalContext());
}

CodeGenContext::~CodeGenContext() {
	delete module;
	delete builder;
}

std::map<std::string, llvm::Value*>& CodeGenContext::locals() {
	return blocks.top()->locals;
}

llvm::BasicBlock *CodeGenContext::currentBlock() {
	return blocks.top()->block;
}

void CodeGenContext::pushBlock(llvm::BasicBlock *block) {
	blocks.push(new CodeGenBlock());
	blocks.top()->block = block;
	builder->SetInsertPoint(block);
}

void CodeGenContext::popBlock() {
	CodeGenBlock *top = blocks.top();
	blocks.pop();
	delete top;
	if(blocks.size() > 0) {
		builder->SetInsertPoint(blocks.top()->block);
	}
}

// Compile the AST into a module
void CodeGenContext::generateCode(NExpression& root)
{
	std::cout << "Generating code...\n";

	// Create the top level interpreter function to call as entry
	llvm::ArrayRef<llvm::Type*> argTypes;
	llvm::FunctionType *ftype = llvm::FunctionType::get(
			llvm::Type::getInt32Ty(llvm::getGlobalContext()),
			argTypes,
			false);
	fnMain = llvm::Function::Create(
			ftype,
			llvm::GlobalValue::InternalLinkage,
			"main", module);
	llvm::BasicBlock *bblock = llvm::BasicBlock::Create(
			llvm::getGlobalContext(),
			"entry", fnMain, 0);

	// Push a new variable/block context
	pushBlock(bblock);
	llvm::Value* valRet = root.codeGen(*this); // emit bytecode for the toplevel block
	builder->CreateRet(valRet);
	popBlock();

	// Print the bytecode in a human-readable format 
	// to see if our program compiled properly
	std::cout << "Code is generated.\n";
	module->dump();
}

// Executes the AST by running the main function
int CodeGenContext::runCode() {
	std::cout << "Running code...\n";

	// build JIT engine
	llvm::ExecutionEngine *ee =
		llvm::EngineBuilder(std::unique_ptr<llvm::Module>(module))
		.create();

	// get main function and execute
	// llvm::ExecutionEngine::getPointerToFunction is deprecated in ver3.7
	int (*fpMain)() = (int (*)())ee->getFunctionAddress("main");
	auto valRet = fpMain();

	std::cout << "Code was run.\n";
	return valRet;
}

void CodeGenContext::addCoreFunctions(llvm::Function *fn) {

}

/*
 * LLVM IR generation part
 */

llvm::Value* NInteger::codeGen(CodeGenContext& context) {
	return llvm::ConstantInt::get(
			llvm::Type::getInt32Ty(llvm::getGlobalContext()),
			value,
			true);
}

llvm::Value* NBoolean::codeGen(CodeGenContext& context) {
	if(value == true) {
		return llvm::ConstantInt::getTrue(llvm::getGlobalContext());
	} else {
		return llvm::ConstantInt::getFalse(llvm::getGlobalContext());
	}
}

llvm::Value* NFloat::codeGen(CodeGenContext& context) {
	return llvm::ConstantFP::get(
			llvm::Type::getDoubleTy(llvm::getGlobalContext()), value);
}

llvm::Value* NIdentifier::codeGen(CodeGenContext& context) {
	llvm::Value* ptr = context.locals()[name];
#ifdef LEML_DEBUG
	assert(ptr != nullptr);
#endif
	return context.builder->CreateLoad(ptr, name);
}

llvm::Value* NUnaryExpression::codeGen(CodeGenContext& context) {
	switch(op) {
		case LNeg:
		{
			return context.builder->CreateSub(
				llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0),
				expr.codeGen(context));
		}
		case LFNeg:
		{
			return context.builder->CreateFSub(
				llvm::ConstantInt::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()), 0),
				expr.codeGen(context));
		}
		default: return nullptr;
	}
}

llvm::Value* NBinaryExpression::codeGen(CodeGenContext& context) {
	llvm::Instruction::BinaryOps instr;

	switch(op) {
		case LAdd:  instr = llvm::Instruction::Add; break;
		case LSub:  instr = llvm::Instruction::Sub; break;
		case LMul:  instr = llvm::Instruction::Mul; break;
		case LDiv:  instr = llvm::Instruction::SDiv; break;
		case LFAdd: instr = llvm::Instruction::FAdd; break;
		case LFSub: instr = llvm::Instruction::FSub; break;
		case LFMul: instr = llvm::Instruction::FMul; break;
		case LFDiv: instr = llvm::Instruction::FDiv; break;
		default:         return nullptr;
	}

	return context.builder->CreateBinOp(
			instr,
			lhs.codeGen(context),
			rhs.codeGen(context));
}

llvm::Value* NCompExpression::codeGen(CodeGenContext& context) {
	llvm::CmpInst::Predicate pred;

	switch(op) {
		case LEq:  pred = llvm::CmpInst::ICMP_EQ;  break;
		case LNeq: pred = llvm::CmpInst::ICMP_NE;  break;
		case LLT:  pred = llvm::CmpInst::ICMP_SLT; break;
		case LLE:  pred = llvm::CmpInst::ICMP_SLE; break;
		case LGT:  pred = llvm::CmpInst::ICMP_SGT; break;
		case LGE:  pred = llvm::CmpInst::ICMP_SGE; break;
		default:             return nullptr;
	}

	llvm::Value* valCmp = context.builder->CreateICmp(
			pred,
			lhs.codeGen(context),
			rhs.codeGen(context),
			"cmp");

	// convert comparing result (i1) to i32
	return context.builder->CreateZExtOrBitCast(
			valCmp,
			llvm::Type::getInt32Ty(llvm::getGlobalContext()),
			"bool2int");
}

llvm::Value* NIfExpression::codeGen(CodeGenContext& context) {
	llvm::Value* valCond = cond.codeGen(context);
	if(!valCond) return nullptr;

	// convert condition to a boolean value by comparing equal to 0
	valCond = context.builder->CreateICmpNE(
			valCond,
			llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0),
			"if.cond");

	llvm::Function* fn = context.builder->GetInsertBlock()->getParent();

	// create three llvm::BasicBlocks and insert then-clause first
	llvm::BasicBlock* blkThen = llvm::BasicBlock::Create(
			llvm::getGlobalContext(), "if.then", fn);
	llvm::BasicBlock* blkElse = llvm::BasicBlock::Create(
			llvm::getGlobalContext(), "if.else");
	llvm::BasicBlock* blkCont = llvm::BasicBlock::Create(
			llvm::getGlobalContext(), "if.cont");

	context.builder->CreateCondBr(valCond, blkThen, blkElse);

	// emit then-block
	context.builder->SetInsertPoint(blkThen);
	llvm::Value* valThen = true_exp.codeGen(context);
	if(!valThen) return nullptr;
	context.builder->CreateBr(blkCont);
	blkThen = context.builder->GetInsertBlock();

	// emit else-block
	fn->getBasicBlockList().push_back(blkElse);
	context.builder->SetInsertPoint(blkElse);
	llvm::Value* valElse = false_exp.codeGen(context);
	if(!valElse) return nullptr;
	context.builder->CreateBr(blkCont);
	blkElse = context.builder->GetInsertBlock();

	// emit if-continue-block
	fn->getBasicBlockList().push_back(blkCont);
	context.builder->SetInsertPoint(blkCont);
	llvm::PHINode* pn = context.builder->CreatePHI(
			llvm::Type::getInt32Ty(llvm::getGlobalContext()),
			2, "if.tmp");
	pn->addIncoming(valThen, blkThen);
	pn->addIncoming(valElse, blkElse);

	return pn;
}

llvm::Value* NAssignment::codeGen(CodeGenContext& context) {
	if (context.locals().find(lhs.name) == context.locals().end()) {
		std::cerr << "undeclared variable " << lhs.name << std::endl;
		return NULL;
	}
	return new llvm::StoreInst(rhs.codeGen(context), context.locals()[lhs.name], false, context.currentBlock());
}

llvm::Value* NLetExpression::codeGen(CodeGenContext& context) {
	llvm::AllocaInst *alloc = new llvm::AllocaInst(llvm::Type::getInt32Ty(llvm::getGlobalContext()), id.name.c_str(), context.currentBlock());
	context.locals()[id.name] = alloc;

	if(assign != nullptr) {
		NAssignment assn(id, *assign);
		assn.codeGen(context);
		return eval->codeGen(context);
	} else {
		return alloc;
	}
}

llvm::Value* NLetRecExpression::codeGen(CodeGenContext& context) {
	// build function prototype
	t = deref(t);
	std::vector<llvm::Type*> argtypes;
	for(auto ty: t->array) {
		argtypes.push_back(llvmType(ty));
	}
	// t->data: type of this function
	// t->data->data: wrapper of ret value type
	// t->data->data->data: ret value type
	auto* typeRet = t->data;
	llvm::FunctionType* ftype = llvm::FunctionType::get(
			llvmType(typeRet),
			makeArrayRef(argtypes), false);
	llvm::Function* fn = llvm::Function::Create(
			ftype, llvm::GlobalValue::InternalLinkage,
			id.name.c_str(), context.module);
	llvm::BasicBlock* bblock = llvm::BasicBlock::Create(
			llvm::getGlobalContext(), "entry", fn, 0);

	context.pushBlock(bblock);

	// generate function body code

	// build arguments instances
	auto argValues = fn->arg_begin();
	llvm::Value* argValue = nullptr;
	for(auto arg: args) {
		arg->codeGen(context);
		argValue = argValues++;
		argValue->setName(arg->id.name);
		context.builder->CreateStore(argValue, context.locals()[arg->id.name]);
	}

	// build function body
	llvm::Value* valRet = body.codeGen(context);

	// emit return value
	context.builder->CreateRet(valRet);

	context.popBlock();

	return eval.codeGen(context);
}

llvm::Value* NFundefExpression::codeGen(CodeGenContext& context) {
#ifdef LEML_DEBUG
	// do not call NFundefExpression::codeGen
	assert(false);
#endif
	return nullptr;
}

llvm::Value* NCallExpression::codeGen(CodeGenContext& context) {
	// get function
	NIdentifier* id = dynamic_cast<NIdentifier*>(&fun);
	assert(id != nullptr); // TODO: higher-order function
	llvm::Function *fn = context.module->getFunction(id->name.c_str());
	assert(fn != nullptr);

	// generate code of arguments
	std::vector<llvm::Value*> argValues;
	for(auto arg: *args) {
		argValues.push_back(arg->codeGen(context));
	}
	return context.builder->CreateCall(fn, argValues);
}

llvm::Value* NArrayExpression::codeGen(CodeGenContext& context) {
	llvm::Value* arrayLength = length.codeGen(context);
	llvm::Value* valData = data.codeGen(context);
	return context.builder->CreateAlloca(llvm::Type::getInt32Ty(llvm::getGlobalContext()), arrayLength);
}
