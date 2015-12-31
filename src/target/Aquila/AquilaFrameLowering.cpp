#include "AquilaFrameLowering.h"
#include "AquilaInstrInfo.h"
#include "AquilaMachineFunction.h"
#include "MCTargetDesc/AquilaMCTargetDesc.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

bool AquilaFrameLowering::
hasFP(const MachineFunction &MF) const {
	return false;
}

void AquilaFrameLowering::
emitPrologue(MachineFunction &MF) const {
	DEBUG(dbgs() << ">> AquilaFrameLowering::emitPrologue <<\n");

	MachineBasicBlock &MBB   = MF.front();
	MachineFrameInfo *MFI = MF.getFrameInfo();

	const AquilaInstrInfo &TII =
		*static_cast<const AquilaInstrInfo*>(MF.getTarget().getInstrInfo());

	MachineBasicBlock::iterator MBBI = MBB.begin();
	DebugLoc dl = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();

	// allocate fixed size for simplicity
	uint64_t StackSize = 4 * 16;

	// Update stack size
	MFI->setStackSize(StackSize);

	BuildMI(MBB, MBBI, dl, TII.get(Aquila::MOVE), Aquila::R20)
		.addImm(-StackSize);
	BuildMI(MBB, MBBI, dl, TII.get(Aquila::ADD), Aquila::R7)
		.addReg(Aquila::R7)
		.addReg(Aquila::R20);
}

void AquilaFrameLowering::
emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const {
	DEBUG(dbgs() << ">> AquilaFrameLowering::emitEpilogue <<\n");

	MachineBasicBlock::iterator MBBI = MBB.getLastNonDebugInstr();
	MachineFrameInfo *MFI            = MF.getFrameInfo();
	const AquilaInstrInfo &TII =
		*static_cast<const AquilaInstrInfo*>(MF.getTarget().getInstrInfo());
	DebugLoc dl = MBBI->getDebugLoc();

	// Get the number of bytes from FrameInfo
	uint64_t StackSize = MFI->getStackSize();

	// Adjust stack.
	BuildMI(MBB, MBBI, dl, TII.get(Aquila::MOVE), Aquila::R20)
		.addImm(StackSize);
	BuildMI(MBB, MBBI, dl, TII.get(Aquila::ADD), Aquila::R7)
		.addReg(Aquila::R7)
		.addReg(Aquila::R20);
}
