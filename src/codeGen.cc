#include <cstdlib>
#include <algorithm>
#include "leml.hh"
#include "codeGen.hh"
#include "syntax.hh"
#include "parser.hh"
#include "infer.hh"
#include "error.hh"

/*
 * the definition of CodeGenContext class
 */

CodeGenContext::CodeGenContext() {
	module = llvm::make_unique<llvm::Module>(gFilename, llvm::getGlobalContext());

	builder = llvm::make_unique<llvm::IRBuilder<>>(llvm::getGlobalContext());

	// initialize pass manager
	fpm = llvm::make_unique<llvm::FunctionPassManager>(module.get());
	if(gMem2reg) fpm->add(llvm::createPromoteMemoryToRegisterPass());
	fpm->doInitialization();
}

CodeGenContext::~CodeGenContext() {}

std::map<std::string, llvm::Value*>& CodeGenContext::locals() {
	return blocks.top()->locals;
}

llvm::BasicBlock *CodeGenContext::currentBlock() {
	return blocks.top()->block;
}

void CodeGenContext::setCurrentBlock(llvm::BasicBlock *block) {
	builder->SetInsertPoint(block);
	blocks.top()->block = block;
}

llvm::BasicBlock *CodeGenContext::getCurrentBlock() {
	return blocks.top()->block;
}

void CodeGenContext::pushBlock(llvm::BasicBlock *block) {
	blocks.push(llvm::make_unique<CodeGenBlock>(block));
	builder->SetInsertPoint(block);
}

llvm::BasicBlock *CodeGenContext::popBlock() {
	auto bblock = currentBlock();
	blocks.pop();
	if(blocks.size() > 0) {
		builder->SetInsertPoint(blocks.top()->block);
	}
	return bblock;
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
bool CodeGenContext::generateCode(NExpression& root, std::unique_ptr<LemlType> type) {

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
	if(!gNostdlib && !linkModule(module.get(), builtinIRFileName)) {
		std::cerr << "error: external library link failure" << std::endl;
		return false;
	}

	fpm->run(*fnMain);

	return true;
}

// Executes the AST by running the main function
void CodeGenContext::runCode() {

	// build JIT engine
	llvm::ExecutionEngine *ee =
		llvm::EngineBuilder(std::unique_ptr<llvm::Module>(module.get()))
		.create();

	// get main function and execute
	// llvm::ExecutionEngine::getPointerToFunction is deprecated in ver3.6
	int (*fpMain)() = (int (*)())ee->getFunctionAddress("main");
	auto valRet = fpMain();

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
	if(!ptr) {
		auto fn = context.module->getFunction(name.c_str());
		if(!fn) {
			std::cerr << "error: undefined reference `" << name << "`" << std::endl;
			std::exit(EXIT_FAILURE);
		} else {
			return fn;
		}
	}
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
				llvm::ConstantFP::get(llvm::Type::getDoubleTy(llvm::getGlobalContext()), 0),
				expr.codeGen(context));
		}
		case LNot:
		{
			return context.builder->CreateSub(
				llvm::ConstantInt::get(llvm::Type::getInt1Ty(llvm::getGlobalContext()), 1),
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
	llvm::Value* valCmp = nullptr;

	if(t == typeInt) {
		switch(op) {
			case LEq:  pred = llvm::CmpInst::ICMP_EQ;  break;
			case LNeq: pred = llvm::CmpInst::ICMP_NE;  break;
			case LLT:  pred = llvm::CmpInst::ICMP_SLT; break;
			case LLE:  pred = llvm::CmpInst::ICMP_SLE; break;
			case LGT:  pred = llvm::CmpInst::ICMP_SGT; break;
			case LGE:  pred = llvm::CmpInst::ICMP_SGE; break;
			default:             return nullptr;
		}

		valCmp = context.builder->CreateICmp(
				pred,
				lhs.codeGen(context),
				rhs.codeGen(context),
				"cmp");
	} else if(t == typeFloat) {
		switch(op) {
			case LEq:  pred = llvm::FCmpInst::FCMP_OEQ;  break;
			case LNeq: pred = llvm::FCmpInst::FCMP_ONE;  break;
			case LLT:  pred = llvm::FCmpInst::FCMP_OLT; break;
			case LLE:  pred = llvm::FCmpInst::FCMP_OLE; break;
			case LGT:  pred = llvm::FCmpInst::FCMP_OGT; break;
			case LGE:  pred = llvm::FCmpInst::FCMP_OGE; break;
			default:             return nullptr;
		}

		valCmp = context.builder->CreateFCmp(
				pred,
				lhs.codeGen(context),
				rhs.codeGen(context),
				"fcmp");
	} else {
#ifdef LEML_DEBUG
		std::cerr << *t << std::endl;
#endif
		assert(false);
	}

	// convert comparing result (i1) to i32
	return valCmp;
}

llvm::Value* NIfExpression::codeGen(CodeGenContext& context) {
	llvm::Value* valCond = cond.codeGen(context);
	if(!valCond) return nullptr;

	// convert condition to a boolean value by comparing equal to 0
	valCond = context.builder->CreateICmpNE(
			valCond,
			llvm::ConstantInt::get(llvm::Type::getInt1Ty(llvm::getGlobalContext()), 0),
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
	context.setCurrentBlock(blkThen);
	llvm::Value* valThen = true_exp.codeGen(context);
	if(!valThen) return nullptr;
	context.builder->CreateBr(blkCont);
	blkThen = context.getCurrentBlock();

	// emit else-block
	fn->getBasicBlockList().push_back(blkElse);
	context.setCurrentBlock(blkElse);
	llvm::Value* valElse = false_exp.codeGen(context);
	if(!valElse) return nullptr;
	context.builder->CreateBr(blkCont);
	blkElse = context.getCurrentBlock();

	// emit if-continue-block
	fn->getBasicBlockList().push_back(blkCont);
	context.setCurrentBlock(blkCont);

	if(valThen->getType() == llvm::Type::getVoidTy(llvm::getGlobalContext()) || valElse->getType() == llvm::Type::getVoidTy(llvm::getGlobalContext())) {
		// if then-block and else-block returns void
		// then manually return void
		// (because PHIInst cannot handle void)
		return llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0);
	} else {
		llvm::PHINode* pn = context.builder->CreatePHI(
				valThen->getType(), 2, "if.tmp");
		pn->addIncoming(valThen, blkThen);
		pn->addIncoming(valElse, blkElse);
		return pn;
	}
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
			if(valAssign->getType() != llvm::Type::getVoidTy(llvm::getGlobalContext())) {
				// bind `assign` to variable unless its type is void
				context.builder->CreateStore(
						valAssign,
						alloc);
			}
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
	proto->t = t;
	if(gVerbose) {
		std::cerr << proto->id.name << ": " << *t << std::endl;
	}
	llvm::Function* fn = static_cast<llvm::Function*>(proto->codeGen(context));

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
	if(valRet->getType() != llvm::Type::getVoidTy(llvm::getGlobalContext())) {
		context.builder->CreateRet(valRet);
	} else {
		// return void
		context.builder->CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0));
	}

	context.popBlock();

	context.fpm->run(*fn);

	return eval.codeGen(context);
}

llvm::Value* NFundefExpression::codeGen(CodeGenContext& context) {
	// infer function type
	std::vector<llvm::Type*> argtypes;
	for(auto ty: t->array) {
		argtypes.push_back(llvmType(ty));
	}
	auto* typeRet = t->data;
	llvm::FunctionType* ftype = llvm::FunctionType::get(
			llvmType(typeRet),
			makeArrayRef(argtypes), false);
	
	// create function
	llvm::Function* fn = llvm::Function::Create(
			ftype, llvm::GlobalValue::ExternalLinkage,
			id.name.c_str(), context.module.get());
	return fn;
}

llvm::Value* NCallExpression::codeGen(CodeGenContext& context) {
	// get function
	NIdentifier* id = dynamic_cast<NIdentifier*>(&fun);
	llvm::Function *fn;
	std::vector<llvm::Value*> argValues;

	if(id) {
		fn = context.module->getFunction(id->name.c_str()); 

		if(fn == nullptr) {
			std::cerr << "error: unknown reference to function `" << id->name.c_str() << "`" << std::endl;
			return nullptr;
		}

	} else {
		// higher-order function
		fn = static_cast<llvm::Function*>(fun.codeGen(context));
	}

	for(auto arg: *args) {
		auto v = arg->codeGen(context);
		argValues.push_back(v);
	}

	// generate code of arguments
	return context.builder->CreateCall(fn, argValues);
}

llvm::Value* NArrayExpression::codeGen(CodeGenContext& context) {
	llvm::Value* arrayLength = length.codeGen(context);
	llvm::Value* valData = data.codeGen(context);

	// array allocation
	auto llvmTy = llvmType(t);
	auto ITy = llvm::Type::getInt32Ty(llvm::getGlobalContext());
	auto allocSize = llvm::ConstantExpr::getTruncOrBitCast(
			llvm::ConstantExpr::getSizeOf(llvmTy), ITy);
	auto array = llvm::CallInst::CreateMalloc(
			context.getCurrentBlock(), ITy,
			llvmTy, allocSize, arrayLength, nullptr, "array");
	context.getCurrentBlock()->getInstList().push_back(array);

	// array initialization
	auto index = context.builder->CreateAlloca(llvm::Type::getInt32Ty(llvm::getGlobalContext()));
	context.builder->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0), index);

	llvm::Function* fn = context.builder->GetInsertBlock()->getParent();

	// create three llvm::BasicBlocks and insert for-cond-clause first
	llvm::BasicBlock* blkCond = llvm::BasicBlock::Create(
			llvm::getGlobalContext(), "for.cond", fn);
	llvm::BasicBlock* blkBody = llvm::BasicBlock::Create(
			llvm::getGlobalContext(), "for.body");
	llvm::BasicBlock* blkEnd = llvm::BasicBlock::Create(
			llvm::getGlobalContext(), "for.end");

	context.builder->CreateBr(blkCond);

	// emit for-cond
	context.setCurrentBlock(blkCond);
	auto valCond = context.builder->CreateICmp(
			llvm::CmpInst::ICMP_SLT,
			context.builder->CreateLoad(index),
			arrayLength, "cmp");
	context.builder->CreateCondBr(valCond, blkBody, blkEnd);
	blkCond = context.getCurrentBlock();

	// emit for-body
	fn->getBasicBlockList().push_back(blkBody);
	context.setCurrentBlock(blkBody);
	// store data to array at current index
	auto ptr = context.builder->CreateGEP(
			array,
			llvm::ArrayRef<llvm::Value*>(context.builder->CreateLoad(index)));
	context.builder->CreateStore(valData, ptr);
	// increment index
	auto updatedIndex = context.builder->CreateBinOp(
			llvm::Instruction::Add,
			context.builder->CreateLoad(index),
			llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 1));
	context.builder->CreateStore(updatedIndex, index);
	// jump to for-cond
	context.builder->CreateBr(blkCond);
	blkBody = context.getCurrentBlock();

	// emit for-end
	fn->getBasicBlockList().push_back(blkEnd);
	context.setCurrentBlock(blkEnd);

	return array;
}

llvm::Value* NArrayGetExpression::codeGen(CodeGenContext& context) {
	llvm::Value *arr;
	if(typeid(array) == typeid(NIdentifier)) {
		auto array_alloc = context.locals()[array.name];
#ifdef LEML_DEBUG
		assert(array_alloc != nullptr);
#endif
		arr = context.builder->CreateLoad(array_alloc);
	} else {
		arr = array.codeGen(context);
	}
	auto i = index.codeGen(context);
	auto indexList = llvm::ArrayRef<llvm::Value*>(i);
	auto ptr = context.builder->CreateGEP(arr, indexList);
	return context.builder->CreateLoad(ptr);
}

llvm::Value* NArrayPutExpression::codeGen(CodeGenContext& context) {
	llvm::Value *arr;
	if(typeid(array) == typeid(NIdentifier)) {
		auto array_alloc = context.locals()[array.name];
#ifdef LEML_DEBUG
		assert(array_alloc != nullptr);
#endif
		arr = context.builder->CreateLoad(array_alloc);
	} else {
		arr = array.codeGen(context);
	}
	auto ptr = context.builder->CreateGEP(arr, llvm::ArrayRef<llvm::Value*>(index.codeGen(context)));
	return context.builder->CreateStore(exp.codeGen(context), ptr);
}

llvm::Value* NTupleExpression::codeGen(CodeGenContext& context) {

#ifdef LEML_DEBUG
	vectorLengthAssertion(this->elems, this->types);
#endif

	std::vector<llvm::Type*> types(this->types.size());
	std::transform(this->types.begin(), this->types.end(), types.begin(),
			[](LemlType *t) { return llvmType(t); });

	// build tuple type as struct
	auto type = llvm::StructType::get(
			llvm::getGlobalContext(),
			llvm::makeArrayRef(types));

	// tuple allocation
	auto ITy = llvm::Type::getInt32Ty(llvm::getGlobalContext());
	auto allocSize = llvm::ConstantExpr::getTruncOrBitCast(
			llvm::ConstantExpr::getSizeOf(type), ITy);
	auto alloc = llvm::CallInst::CreateMalloc(
			context.getCurrentBlock(), ITy,
			type, allocSize, nullptr, nullptr, "tuple");
	context.getCurrentBlock()->getInstList().push_back(alloc);

	auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0);
	for(unsigned long i = 0; i < elems.size(); i++) {
		NInteger index = NInteger(i);
		auto elem = elems[i];
		// get a pointer to each element
		auto ptr = context.builder->CreateGEP(
				alloc,
				llvm::ArrayRef<llvm::Value*>({zero, index.codeGen(context)}));

		// store each element
		context.builder->CreateStore(elem->codeGen(context), ptr);
	}

	return alloc;
}

llvm::Value* NLetTupleExpression::codeGen(CodeGenContext& context) {
	auto tuple_alloc = exp.codeGen(context);
	auto zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(llvm::getGlobalContext()), 0);
	for(unsigned long i = 0; i < ids.size(); i++) {
		NInteger index = NInteger(i);
		auto var = ids[i];
		// get a pointer to each element
		auto ptr = context.builder->CreateGEP(
				tuple_alloc,
				llvm::ArrayRef<llvm::Value*>({zero, index.codeGen(context)}));
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
