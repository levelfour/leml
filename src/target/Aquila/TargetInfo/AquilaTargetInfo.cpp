#include "Aquila.h"
#include "llvm/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target llvm::TheAquilaTarget;

extern "C" void LLVMInitializeAquilaTargetInfo() { 
	DEBUG(dbgs() << ">> InitAquilaTargetInfo <<\n");
	RegisterTarget<Triple::aquila, /*HasJIT=*/false>
		X(TheAquilaTarget, "aquila", "Aquila");
}
