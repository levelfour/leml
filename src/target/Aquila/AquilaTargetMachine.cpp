#include "AquilaTargetMachine.h"
#include "Aquila.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

extern "C" void LLVMInitializeAquilaTarget() {
	// Register the target.
	RegisterTargetMachine<AquilaTargetMachine> X(TheAquilaTarget);
}

AquilaTargetMachine::
AquilaTargetMachine(const Target &T, StringRef Triple,
		StringRef CPU, StringRef FS, const TargetOptions &Options,
		Reloc::Model RM, CodeModel::Model CM,
		CodeGenOpt::Level OL)
: LLVMTargetMachine(T, Triple, CPU, FS, Options, RM, CM, OL),
	DL("e-p:32:32:32-i8:8:32-i16:16:32-i64:64:64-n32"),
	Subtarget(Triple, CPU, FS),
	InstrInfo(*this),
	FrameLowering(Subtarget),
	TLInfo(*this), TSInfo(*this) {}

	namespace {
		/// Aquila Code Generator Pass Configuration Options.
		class AquilaPassConfig : public TargetPassConfig {
			public:
				AquilaPassConfig(AquilaTargetMachine *TM, PassManagerBase &PM)
					: TargetPassConfig(TM, PM) {}

				AquilaTargetMachine &getAquilaTargetMachine() const {
					return getTM<AquilaTargetMachine>();
				}

				virtual bool addInstSelector();
		};
	} // namespace

TargetPassConfig *AquilaTargetMachine::createPassConfig(PassManagerBase &PM) {
	return new AquilaPassConfig(this, PM);
}

bool AquilaPassConfig::addInstSelector() {
	// Install an instruction selector.
	addPass(createAquilaISelDag(getAquilaTargetMachine()));
	return false;
}
