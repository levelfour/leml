#include <iostream>
#include "syntax.hh"
#include "infer.hh"
#include "codeGen.hh"

extern NExpression *program;
extern int yyparse();
extern int yydebug;

int main() {

	// initialization of LLVM
	LLVMInitializeNativeTarget();
	LLVMInitializeNativeAsmPrinter();
	LLVMInitializeNativeAsmParser();

	// initialization of bison
#ifdef YYDEBUG
	yydebug = 1;
#endif

	// lexer/parser
	if(yyparse() == 0) {

		// type inference / check
		LemlType* t;
		try {
			t = infer(program, TypeEnv());
			if(t == nullptr) {
				std::cerr << "type check failure" << std::endl;
				exit(EXIT_FAILURE);
			}
		} catch(UnificationError e) {
			std::cerr << e.what() << std::endl;
			exit(EXIT_FAILURE);
		}
		t = deref(t);

		// initialize LLVM context
		CodeGenContext context;
//		llvm::Function* fn_printf = createPrintfFunction(context);
//		llvm::Function* fn_print_int = createPrintIntFunction(context, fn_printf);
//		context.addCoreFunctions(fn_print_int);

		// generate LLVM IR
		context.generateCode(*program, t);
		std::cout << *program << std::endl;

		// run on LLVM JIT
		auto valRet = context.runCode();
		std::cout << "return value = " << valRet
				  << ", type = " << *t << std::endl;
	}

	return 0;
}


llvm::Function* createPrintfFunction(CodeGenContext& context) {
    std::vector<llvm::Type*> printf_arg_types;
    printf_arg_types.push_back(llvm::Type::getInt8PtrTy(llvm::getGlobalContext())); //char*

    llvm::FunctionType* printf_type =
        llvm::FunctionType::get(
            llvm::Type::getInt32Ty(llvm::getGlobalContext()), printf_arg_types, true);

    llvm::Function *func = llvm::Function::Create(
                printf_type, llvm::Function::ExternalLinkage,
                llvm::Twine("printf"),
                context.module
           );
    func->setCallingConv(llvm::CallingConv::C);
    return func;
}

llvm::Function* createPrintIntFunction(CodeGenContext& context, llvm::Function* fn_printf) {
	std::vector<llvm::Type*> arg_types;
	arg_types.push_back(llvm::Type::getInt32Ty(llvm::getGlobalContext()));

	llvm::FunctionType* fn_type =
		llvm::FunctionType::get(
				llvm::Type::getVoidTy(llvm::getGlobalContext()), arg_types, true);

	llvm::Function* func = llvm::Function::Create(
			fn_type, llvm::Function::InternalLinkage,
			llvm::Twine("print_int"),
			context.module
			);

	llvm::BasicBlock* bblock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", func, 0);

	const char *formatStr = "%d\n";	
	llvm::Constant* formatConst = llvm::ConstantDataArray::getString(llvm::getGlobalContext(), formatStr);
	llvm::GlobalVariable *var =
		new llvm::GlobalVariable(
				*context.module,
				llvm::ArrayType::get(llvm::IntegerType::get(llvm::getGlobalContext(), 8), strlen(formatStr)+1),
				true, llvm::GlobalVariable::PrivateLinkage,
				formatConst, ".str");

	llvm::Constant* zero = llvm::Constant::getNullValue(llvm::IntegerType::getInt32Ty(llvm::getGlobalContext()));

	std::vector<llvm::Constant*> indices;
	indices.push_back(zero);
	indices.push_back(zero);
	llvm::Constant* varRef = llvm::ConstantExpr::getGetElementPtr(var, indices);

	std::vector<llvm::Value*> args;
	args.push_back(varRef);

	llvm::Function::arg_iterator argsValues = func->arg_begin();
	llvm::Value* toPrint = argsValues++;
	toPrint->setName("toPrint");
	args.push_back(toPrint);

	//llvm::CallInst* call = llvm::CallInst::Create(fn_printf, makeArrayRef(args), "", bblock);
	llvm::ReturnInst::Create(llvm::getGlobalContext(), bblock);
	context.popBlock();

	return func;
}

