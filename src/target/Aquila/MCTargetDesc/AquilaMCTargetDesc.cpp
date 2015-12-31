#include "AquilaMCAsmInfo.h"
#include "AquilaMCTargetDesc.h"
#include "InstPrinter/AquilaInstPrinter.h"
#include "llvm/MC/MachineLocation.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "AquilaGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "AquilaGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "AquilaGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createAquilaMCInstrInfo() {
	MCInstrInfo *X = new MCInstrInfo();
	InitAquilaMCInstrInfo(X);
	return X;
}

static MCRegisterInfo *createAquilaMCRegisterInfo(StringRef TT) {
	MCRegisterInfo *X = new MCRegisterInfo();
	InitAquilaMCRegisterInfo(X, Aquila::RA);
	return X;
}

static MCSubtargetInfo *createAquilaMCSubtargetInfo(StringRef TT, StringRef CPU,
		StringRef FS) {
	MCSubtargetInfo *X = new MCSubtargetInfo();
	InitAquilaMCSubtargetInfo(X, TT, CPU, FS);
	return X;
}

static MCAsmInfo *createAquilaMCAsmInfo(const Target &T, StringRef TT) {
	MCAsmInfo *MAI = new AquilaMCAsmInfo(T, TT);

	MachineLocation Dst(MachineLocation::VirtualFP);
	MachineLocation Src(Aquila::SP, 0);
	MAI->addInitialFrameState(0, Dst, Src);

	return MAI;
}

static MCCodeGenInfo *createAquilaMCCodeGenInfo(StringRef TT, Reloc::Model RM,
		CodeModel::Model CM,
		CodeGenOpt::Level OL) {
	MCCodeGenInfo *X = new MCCodeGenInfo();
	X->InitMCCodeGenInfo(RM, CM, OL);
	return X;
}

static MCInstPrinter *createAquilaMCInstPrinter(const Target &T,
		unsigned SyntaxVariant,
		const MCAsmInfo &MAI,
		const MCInstrInfo &MII,
		const MCRegisterInfo &MRI,
		const MCSubtargetInfo &STI) {
	return new AquilaInstPrinter(MAI, MII, MRI);
}

static MCStreamer *createMCStreamer(const Target &T, StringRef TT,
		MCContext &Ctx, MCAsmBackend &MAB,
		raw_ostream &_OS,
		MCCodeEmitter *_Emitter,
		bool RelaxAll,
		bool NoExecStack) {
	Triple TheTriple(TT);

	return createELFStreamer(Ctx, MAB, _OS, _Emitter, RelaxAll, NoExecStack);
}

extern "C" void LLVMInitializeAquilaTargetMC() {
	// Register the MC asm info.
	RegisterMCAsmInfoFn X(TheAquilaTarget, createAquilaMCAsmInfo);
	// Register the MC codegen info.
	TargetRegistry::RegisterMCCodeGenInfo(TheAquilaTarget,
			createAquilaMCCodeGenInfo);
	// Register the MC instruction info.
	TargetRegistry::RegisterMCInstrInfo(TheAquilaTarget, createAquilaMCInstrInfo);
	// Register the MC register info.
	TargetRegistry::RegisterMCRegInfo(TheAquilaTarget, createAquilaMCRegisterInfo);
	// Register the MC Code Emitter
	TargetRegistry::RegisterMCCodeEmitter(TheAquilaTarget,
			createAquilaMCCodeEmitter);
	// Register the object streamer.
	TargetRegistry::RegisterMCObjectStreamer(TheAquilaTarget, createMCStreamer);
	// Register the asm backend.
	TargetRegistry::RegisterMCAsmBackend(TheAquilaTarget,
			createAquilaAsmBackend);
	// Register the MC subtarget info.
	TargetRegistry::RegisterMCSubtargetInfo(TheAquilaTarget,
			createAquilaMCSubtargetInfo);
	// Register the MCInstPrinter.
	TargetRegistry::RegisterMCInstPrinter(TheAquilaTarget,
			createAquilaMCInstPrinter);
}
