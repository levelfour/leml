#define DEBUG_TYPE "asm-printer"
#include "Aquila.h"
#include "AquilaInstrInfo.h"
#include "AquilaMCInstLower.h"
#include "AquilaTargetMachine.h"
#include "InstPrinter/AquilaInstPrinter.h"
#include "llvm/Constants.h"
#include "llvm/DerivedTypes.h"
#include "llvm/Module.h"
#include "llvm/Assembly/Writer.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Target/Mangler.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

namespace {
	class AquilaAsmPrinter : public AsmPrinter {
		public:
			AquilaAsmPrinter(TargetMachine &TM, MCStreamer &Streamer)
				: AsmPrinter(TM, Streamer) {}

			virtual const char *getPassName() const {
				return "Aquila Assembly Printer";
			}

			// should overwrite functions
			void EmitInstruction(const MachineInstr *MI) /*override*/;
	};
} // end of anonymous namespace

void AquilaAsmPrinter::
EmitInstruction(const MachineInstr *MI) {
	DEBUG(dbgs() << ">> AquilaAsmPinter::EmitInstruction <<\n");
	DEBUG(MI->dump());
	AquilaMCInstLower MCInstLowering(OutContext, *Mang, *this);
	MCInst TmpInst;
	MCInstLowering.Lower(MI, TmpInst);
	OutStreamer.EmitInstruction(TmpInst);
}

// Force static initialization.
extern "C" void LLVMInitializeAquilaAsmPrinter() {
	RegisterAsmPrinter<AquilaAsmPrinter> X(TheAquilaTarget);
}
