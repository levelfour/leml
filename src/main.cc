#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include <llvm/IR/IRPrintingPasses.h>

#include "cmdopt.hh"
#include "syntax.hh"
#include "infer.hh"
#include "codeGen.hh"

extern NExpression *program;
extern int yyparse();
extern int yydebug;
extern FILE* yyin;

void JITExecution(CodeGenContext& context, std::string filename, std::string type, bool verbose = false);
void IREmission(CodeGenContext& context, std::string filename);

int main(int argc, char** argv) {

	// option parser
	OptionParser o(argc, argv);
	std::map<std::string, int> spec;
	spec["jit"]  = 0; // JIT
	spec["o"]    = 1; // output file name
	spec["v"]    = 0; // verbose
	spec["type"] = 1; // result value type
	o.set(spec);
	o.build();

	// initialization of LLVM
	LLVMInitializeNativeTarget();
	LLVMInitializeNativeAsmPrinter();
	LLVMInitializeNativeAsmParser();

	// initialization of bison
#ifdef YYDEBUG
	yydebug = 1;
#endif

	// open file
	if(o.get("default") != "") {
		FILE* ifs = fopen(o.get("default").c_str(), "r");
		yyin = ifs;
	}

	// lexer/parser
	if(yyparse() == 0) {

		// type inference / check
		std::unique_ptr<LemlType> t(check(program));

		// initialize LLVM context
		CodeGenContext context;
//		llvm::Function* fn_printf = createPrintfFunction(context);
//		llvm::Function* fn_print_int = createPrintIntFunction(context, fn_printf);
//		context.addCoreFunctions(fn_print_int);

		// generate LLVM IR
		context.generateCode(*program, std::move(t), o.get("v") != "");

		if(o.get("v") != "") {
			std::cout << *program << std::endl;
		}

		if(o.get("jit") != "") {
			JITExecution(context, o.get("o"), o.get("type"), o.get("v") != "");
		} else {
			IREmission(context, o.get("o"));
		}
	}

	return 0;
}

void JITExecution(CodeGenContext& context, std::string filename, std::string type, bool verbose) {
	// run on LLVM JIT
	std::stringstream ss;
	context.runCode(verbose);
	if(verbose) {
		ss << "return value = ";
	}

	if(type == "float") {
		ss << context.getFloatResult() << std::endl;
	} else {
		ss << context.getIntResult() << std::endl;
	}

	if(filename != "") {
		std::fstream fs;
		fs.open(filename, std::fstream::out);
		fs << ss.str();
	} else {
		std::cout << ss.str();
	}
}

void IREmission(CodeGenContext& context, std::string filename) {
	if(filename == "") {
		// "-" means stdout
		filename = "-";
	}

	std::error_code ec;
	llvm::raw_ostream* out = new llvm::raw_fd_ostream(filename.c_str(), ec, llvm::sys::fs::F_None);

	llvm::PassManager pm;
	pm.add(llvm::createPrintModulePass(*out));
	pm.run(*context.module);
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
                context.module.get()
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
			context.module.get()
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

