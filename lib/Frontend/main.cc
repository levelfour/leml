#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <map>

#include <llvm/IR/IRPrintingPasses.h>

#include "leml.hh"
#include "cmdopt.hh"
#include "syntax.hh"
#include "infer.hh"
#include "codeGen.hh"
#include "lift.hh"

extern NExpression *program;
extern int yyparse();
extern int yydebug;
extern FILE* yyin;

std::string gFilename = "";
bool gVerbose  = false;
bool gNostdlib = false;
bool gMem2reg = false;

void InitEnv(TypeEnv& env);
void JITExecution(CodeGenContext& context, std::string gFilename, std::string type);
void IREmission(CodeGenContext& context, std::string gFilename);

int main(int argc, char** argv) {

	// option parser
	OptionParser o(argc, argv);
	std::map<std::string, int> spec;
	spec["jit"]  = 0; // JIT
	spec["o"]    = 1; // output file name
	spec["v"]    = 0; // gVerbose
	spec["type"] = 1; // result value type
	spec["nostdlib"] = 0;
	spec["mem2reg"] = 0;
	o.set(spec);
	try {
		o.build();
	} catch(FileDoesNotExist ex) {
		std::cerr << "error: `" << ex.what() << "` no such file or directory" << std::endl;
		std::exit(EXIT_FAILURE);
	} catch(std::invalid_argument ex) {
		std::cerr << "error: unknown option `" << ex.what() << "`" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	if(o.get("default") != "")  { gFilename = o.get("default"); }
	if(o.get("v") != "")        { gVerbose = true; }
	if(o.get("nostdlib") != "") { gNostdlib = true; }
	if(o.get("mem2reg") != "")  { gMem2reg = true; }

	// initialization of LLVM
	LLVMInitializeNativeTarget();
	LLVMInitializeNativeAsmPrinter();
	LLVMInitializeNativeAsmParser();

	// initialization of bison
#ifdef YYDEBUG
	yydebug = 1;
#endif

	// open file
	if(gFilename != "") {
		FILE* ifs = fopen(gFilename.c_str(), "r");
		yyin = ifs;
	}

	// lexer/parser
	if(yyparse() == 0) {

		// type inference / check
		TypeEnv env;
		if(!gNostdlib) {
			InitEnv(env);
		}
		std::unique_ptr<LemlType> t(check(program, env));

		// lambda lifting to eliminate closure
		lambdaLifting(env, program);

		// initialize LLVM context
		CodeGenContext context;
		if(!gNostdlib) {
			context.setBuiltInIR(BUILTIN_LIB);
			context.setEnv(env);
		}

		// generate LLVM IR
		if(context.generateCode(
					*program, std::move(t))) {

			if(gVerbose) {
				std::cerr <<
					"-*-*-*-*-*-*-*-*-*-" << std::endl <<
					*program << std::endl <<
					"-*-*-*-*-*-*-*-*-*-" << std::endl;
			}

			if(o.get("jit") != "") {
				JITExecution(context, o.get("o"), o.get("type"));
			} else {
				IREmission(context, o.get("o"));
			}
		}
	}

	return 0;
}

void InitEnv(TypeEnv& env) {
	env["print_int"] = new LemlType({Fun, typeUnit, {typeInt}});
	env["print_float"] = new LemlType({Fun, typeUnit, {typeFloat}});
	env["print_newline"] = new LemlType({Fun, typeUnit, {typeUnit}});
	env["fabs"] = new LemlType({Fun, typeFloat, {typeFloat}});
	env["abs_float"] = new LemlType({Fun, typeFloat, {typeFloat}});
	env["truncate"] = new LemlType({Fun, typeInt, {typeFloat}});
	env["floor"] = new LemlType({Fun, typeFloat, {typeFloat}});
	env["sin"] = new LemlType({Fun, typeFloat, {typeFloat}});
	env["cos"] = new LemlType({Fun, typeFloat, {typeFloat}});
	env["sqrt"] = new LemlType({Fun, typeFloat, {typeFloat}});
	env["int_of_float"] = new LemlType({Fun, typeInt, {typeFloat}});
	env["float_of_int"] = new LemlType({Fun, typeFloat, {typeInt}});
}

void JITExecution(CodeGenContext& context, std::string gFilename, std::string type) {
	// run on LLVM JIT
	std::stringstream ss;
	context.runCode();
	if(gVerbose) {
		ss << "return value = ";
		if(type == "float") {
			ss << context.getFloatResult() << std::endl;
		} else {
			ss << context.getIntResult() << std::endl;
		}
	}

	if(gFilename != "") {
		std::fstream fs;
		fs.open(gFilename, std::fstream::out);
		fs << ss.str();
	} else {
		std::cout << ss.str();
	}
}

void IREmission(CodeGenContext& context, std::string gFilename) {
	if(gFilename == "") {
		// "-" means stdout
		gFilename = "-";
	}

	std::error_code ec;
	llvm::raw_ostream* out = new llvm::raw_fd_ostream(gFilename.c_str(), ec, llvm::sys::fs::F_None);

	llvm::PassManager pm;
	pm.add(llvm::createPrintModulePass(*out));
	pm.run(*context.module);
}

