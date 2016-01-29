#include "codeGen.hh"
#include "syntax.hh"
#include "parser.hh"
#include "infer.hh"

/*
 * the definition of CodeGenContext class
 */

CodeGenContext::CodeGenContext() {
	// TODO: replace "main" with source code name
	module = llvm::make_unique<llvm::Module>("main", llvm::getGlobalContext());

	builder = llvm::make_unique<llvm::IRBuilder<>>(llvm::getGlobalContext());

	// initialize pass manager
	fpm = llvm::make_unique<llvm::FunctionPassManager>(module.get());
	fpm->add(llvm::createPromoteMemoryToRegisterPass());
	fpm->doInitialization();
}

CodeGenContext::~CodeGenContext() {}

std::map<std::string, llvm::Value*>& CodeGenContext::locals() {
	return blocks.top()->locals;
}

llvm::BasicBlock *CodeGenContext::currentBlock() {
	return blocks.top()->block;
}

void CodeGenContext::pushBlock(llvm::BasicBlock *block) {
	blocks.push(llvm::make_unique<CodeGenBlock>(block));
	builder->SetInsertPoint(block);
}

void CodeGenContext::popBlock() {
	blocks.pop();
	if(blocks.size() > 0) {
		builder->SetInsertPoint(blocks.top()->block);
	}
}

void CodeGenContext::setBuiltInIR(std::string filename) {
	builtinIRFileName = filename;
}

void CodeGenContext::setEnv(TypeEnv env) {
	for(auto& kv: env) {
		std::vector<llvm::Type*> argtypes;
		for(auto ty: kv.second->array) {
			argtypes.push_back(llvmType(ty));
		}
		llvm::FunctionType *ft = llvm::FunctionType::get(
				llvmType(kv.second->data),     // return value type
				llvm::makeArrayRef(argtypes),  // arg types
				false);
		llvm::Function::Create(
				ft,
				llvm::GlobalValue::ExternalLinkage, 
				kv.first, // function name
				module.get());
	}
}

// Compile the AST into a module
bool CodeGenContext::generateCode(NExpression& root, std::unique_ptr<LemlType> type, bool nostdlib, bool verbose) {
	if(verbose) std::cout << "Generating code...\n";

	// Create the top level interpreter function to call as entry
	llvm::ArrayRef<llvm::Type*> argTypes;
	typeRet = llvmType(type.get());
	llvm::FunctionType *ftype = llvm::FunctionType::get(
			typeRet, argTypes, false);
	fnMain = llvm::Function::Create(
			ftype,
			llvm::GlobalValue::ExternalLinkage,
			"main", module.get());
	llvm::BasicBlock *bblock = llvm::BasicBlock::Create(
			llvm::getGlobalContext(),
			"entry", fnMain, 0);

	// Push a new variable/block context
	pushBlock(bblock);
	llvm::Value* valRet = root.codeGen(*this); // emit bytecode for the toplevel block
	builder->CreateRet(valRet);
	popBlock();

	// link built-in module
	if(!nostdlib && !linkModule(module.get(), builtinIRFileName)) {
		std::cerr << "error: external library link failure" << std::endl;
		return false;
	}

	fpm->run(*fnMain);

	if(verbose) std::cout << "Code is generated.\n";
	return true;
}

// Executes the AST by running the main function
void CodeGenContext::runCode(bool verbose) {
	if(verbose) std::cout << "Running code...\n";

	// build JIT engine
	llvm::ExecutionEngine *ee =
		llvm::EngineBuilder(std::unique_ptr<llvm::Module>(module.get()))
		.create();

	// get main function and execute
	// llvm::ExecutionEngine::getPointerToFunction is deprecated in ver3.6
	int (*fpMain)() = (int (*)())ee->getFunctionAddress("main");
	auto valRet = fpMain();

	if(verbose) std::cout << "Code was run.\n";

	resultValue.d = valRet;
}

int CodeGenContext::getIntResult() {
	return resultValue.d;
}

float CodeGenContext::getFloatResult() {
	return resultValue.f;
}

bool CodeGenContext::linkModule(llvm::Module *dest, std::string filename) {
	llvm::SMDiagnostic err;
	// load module
	std::unique_ptr<llvm::Module> linkMod(llvm::parseIRFile(filename, err, llvm::getGlobalContext()));
	if(!linkMod) {
		return false;
	}

	// link module
	if(llvm::Linker::LinkModules(dest, linkMod.get())) {
		return false;
	}

	return true;
}

/*
 * LLVM IR generation part
 */

llvm::Value* NUnit::codeGen(CodeGenContext& context) {
	return llvm::ConstantInt::get(
			llvm::Type::getInt32Ty(llvm::getGlobalContext()),
			0,
			true);
}

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

llvm::Value* NLetExpression::codeGen(CodeGenContext& context) {
	auto type = llvmType(t);
	if(type != nullptr) {
		llvm::AllocaInst* alloc = context.builder->CreateAlloca(
				type, nullptr,
				id.name.c_str());
		context.locals()[id.name] = alloc;
	
		if(assign != nullptr) {
			// type of alloc must be a pointer to the type of valAssign
			auto valAssign = assign->codeGen(context);
			context.builder->CreateStore(
					valAssign,
					alloc);
			return eval->codeGen(context);
		} else {
			return alloc;
		}
	} else {
#ifdef LEML_DEBUG
	assert(assign != nullptr);
#endif

		assign->codeGen(context);
		return eval->codeGen(context);
	}
}

llvm::Value* NLetRecExpression::codeGen(CodeGenContext& context) {
	// build function prototype
	t = deref(t);
	std::vector<llvm::Type*> argtypes;
	for(auto ty: t->array) {
		argtypes.push_back(llvmType(ty));
	}
	auto* typeRet = t->data;
	llvm::FunctionType* ftype = llvm::FunctionType::get(
			llvmType(typeRet),
			makeArrayRef(argtypes), false);
	llvm::Function* fn = llvm::Function::Create(
			ftype, llvm::GlobalValue::ExternalLinkage,
			proto->id.name.c_str(), context.module.get());
	llvm::BasicBlock* bblock = llvm::BasicBlock::Create(
			llvm::getGlobalContext(), "entry", fn, 0);

	context.pushBlock(bblock);

	// generate function body code

	// build arguments instances
	auto argValues = fn->arg_begin();
	llvm::Value* argValue = nullptr;
	for(auto arg: proto->args) {
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

	context.fpm->run(*fn);

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

	if(fn == nullptr) {
		std::cerr << "error: unknown reference to function `" << id->name.c_str() << "`" << std::endl;
		return nullptr;
	}

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
	auto typeData = infer(&data, TypeEnv());
	auto array = context.builder->CreateAlloca(
			llvmType(typeData), arrayLength);
	// TODO: array initialization
	auto index = llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0);
	auto ptr = llvm::GetElementPtrInst::Create(
			array, llvm::ArrayRef<llvm::Value*>(index),
			"", context.currentBlock());
	context.builder->CreateStore(valData, ptr);
	return array;
}

llvm::Value* NArrayGetExpression::codeGen(CodeGenContext& context) {
	auto array_alloc = context.locals()[array.name];
#ifdef LEML_DEBUG
	assert(array_alloc != nullptr);
#endif

	auto arr = context.builder->CreateLoad(array_alloc);
	auto ptr = llvm::GetElementPtrInst::Create(
			arr, llvm::ArrayRef<llvm::Value*>(index.codeGen(context)),
			"", context.currentBlock());
	return context.builder->CreateLoad(ptr);
}

llvm::Value* NArrayPutExpression::codeGen(CodeGenContext& context) {
	// get the allocation code of array
	auto array_alloc = context.locals()[array.name];
#ifdef LEML_DEBUG
	assert(array_alloc != nullptr);
#endif

	// get the instance of array
	auto arr = context.builder->CreateLoad(array_alloc);
	auto ptr = llvm::GetElementPtrInst::Create(
			arr, llvm::ArrayRef<llvm::Value*>(index.codeGen(context)),
			"", context.currentBlock());
	return context.builder->CreateStore(exp.codeGen(context), ptr);
}

llvm::Value* NTupleExpression::codeGen(CodeGenContext& context) {
	// build tuple type as struct
	std::vector<llvm::Type*> types;
	for(auto elem: elems) {
		types.push_back(llvmType(infer(elem)));
	}
	auto type = llvm::StructType::get(
			llvm::getGlobalContext(),
			llvm::makeArrayRef(types));

	llvm::AllocaInst* alloc = context.builder->CreateAlloca(type, nullptr);

	auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0);
	for(unsigned long i = 0; i < elems.size(); i++) {
		NInteger index = NInteger(i);
		auto elem = elems[i];
		// get a pointer to each element
		auto ptr = llvm::GetElementPtrInst::Create(
				alloc,
				llvm::ArrayRef<llvm::Value*>({zero, index.codeGen(context)}),
				"", context.currentBlock());

		// store each element
		context.builder->CreateStore(elem->codeGen(context), ptr);
	}

	return alloc;
}

llvm::Value* NLetTupleExpression::codeGen(CodeGenContext& context) {
	NIdentifier* id = dynamic_cast<NIdentifier*>(&exp);
#ifdef LEML_DEBUG
	// assume let rec definition-exp to be an id
	assert(id != nullptr);
#endif

	// get the allocation code of tuple
	auto tuple_alloc = context.locals()[id->name];
#ifdef LEML_DEBUG
	assert(tuple_alloc != nullptr);
#endif

	// get the instance of tuple
	auto tuple = context.builder->CreateLoad(tuple_alloc);

	auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0);
	for(unsigned long i = 0; i < ids.size(); i++) {
		NInteger index = NInteger(i);
		auto var = ids[i];
		// get a pointer to each element
		auto ptr = llvm::GetElementPtrInst::Create(
				tuple,
				llvm::ArrayRef<llvm::Value*>({zero, index.codeGen(context)}),
				"", context.currentBlock());
		auto val = context.builder->CreateLoad(ptr);

		// allocate each decomposed element
		llvm::AllocaInst* alloc = context.builder->CreateAlloca(
				val->getType(), nullptr,
				var->id.name.c_str());
		context.locals()[var->id.name] = alloc;
		context.builder->CreateStore(val, alloc);
	}

	// eval
	return eval.codeGen(context);
}
