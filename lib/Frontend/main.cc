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
		TypeEnv env;
		env["print_int"] = new LemlType({Fun, typeUnit, {typeInt}});
		env["print_float"] = new LemlType({Fun, typeUnit, {typeFloat}});

		std::unique_ptr<LemlType> t(check(program, env));

		// initialize LLVM context
		CodeGenContext context;
		context.setBuiltInIR(BUILTIN_LIB);
		context.setEnv(env);

		// generate LLVM IR
		if(context.generateCode(*program, std::move(t), o.get("v") != "")) {

			if(o.get("v") != "") {
				std::cout <<
					"-*-*-*-*-*-*-*-*-*-" << std::endl <<
					*program << std::endl <<
					"-*-*-*-*-*-*-*-*-*-" << std::endl;
			}

			if(o.get("jit") != "") {
				JITExecution(context, o.get("o"), o.get("type"), o.get("v") != "");
			} else {
				IREmission(context, o.get("o"));
			}
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
		if(type == "float") {
			ss << context.getFloatResult() << std::endl;
		} else {
			ss << context.getIntResult() << std::endl;
		}
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

