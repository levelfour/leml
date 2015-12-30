#define DEBUG_TYPE "aquila-reg-info"

#include "AquilaRegisterInfo.h"
#include "Aquila.h"
#include "llvm/Constants.h"
#include "llvm/Type.h"
#include "llvm/Function.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetInstrInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/DebugInfo.h"

#include "MCTargetDesc/AquilaMCTargetDesc.h"

#define GET_REGINFO_TARGET_DESC
#include "AquilaGenRegisterInfo.inc"

using namespace llvm;

AquilaRegisterInfo::
AquilaRegisterInfo(const TargetInstrInfo &tii)
	: AquilaGenRegisterInfo(Aquila::RA), TII(tii) { }

	//===----------------------------------------------------------------------===//
	// Callee Saved Registers methods
	//===----------------------------------------------------------------------===//

	// 呼び出し先待避レジスタ
	const uint16_t* AquilaRegisterInfo::
	getCalleeSavedRegs(const MachineFunction *MF) const {
		return CSR_SingleFloatOnly_SaveList;
	}

// 呼び出し元待避レジスタ
const uint32_t* AquilaRegisterInfo::
getCallPreservedMask(CallingConv::ID) const {  
	return CSR_SingleFloatOnly_RegMask;
}

BitVector AquilaRegisterInfo::
getReservedRegs(const MachineFunction &MF) const {
	static const uint16_t ReservedCPURegs[] = {
		Aquila::R0, Aquila::R1, Aquila::R2, Aquila::R4,
		Aquila::R5, Aquila::R6, Aquila::R7,
	};

	BitVector Reserved(getNumRegs());
	typedef TargetRegisterClass::iterator RegIter;

	for (unsigned I = 0; I < array_lengthof(ReservedCPURegs); ++I)
		Reserved.set(ReservedCPURegs[I]);

	return Reserved;
}

// ADJCALLSTACKDOWNとADJCALLSTACKUPを単純に削除する
void AquilaRegisterInfo::
eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
		MachineBasicBlock::iterator I) const {
	DEBUG(dbgs() << ">> AquilaRegisterInfo::eliminateCallFramePseudoInstr <<\n";);
	MBB.erase(I);
}

// FrameIndexをスタックポインタに置き換える
void AquilaRegisterInfo::
eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
		RegScavenger *RS) const {
	DEBUG(dbgs() << ">> AquilaRegisterInfo::eliminateFrameIndex <<\n";);

	MachineInstr &MI = *II;
	const MachineFunction &MF = *MI.getParent()->getParent();

	unsigned opIndex;
	for (opIndex = 0; opIndex < MI.getNumOperands(); opIndex++) {
		if (MI.getOperand(opIndex).isFI()) break;
	}
	assert(opIndex < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");

	int FrameIndex = MI.getOperand(opIndex).getIndex();
	uint64_t stackSize = MF.getFrameInfo()->getStackSize();
	int64_t spOffset = MF.getFrameInfo()->getObjectOffset(FrameIndex);
	int64_t Offset = spOffset + stackSize + MI.getOperand(opIndex+1).getImm();
	unsigned FrameReg = Aquila::SP;

	DEBUG(errs() 
			<< "\nFunction : " << MF.getFunction()->getName() << "\n"
			<< "<--------->\n" << MI
			<< "FrameIndex : " << FrameIndex << "\n"
			<< "spOffset   : " << spOffset << "\n"
			<< "stackSize  : " << stackSize << "\n"
			<< "Offset     : " << Offset << "\n" << "<--------->\n");

	DEBUG(errs() << "Before:" << MI);
	MI.getOperand(opIndex).ChangeToRegister(FrameReg, false);
	MI.getOperand(opIndex+1).ChangeToImmediate(Offset);
	DEBUG(errs() << "After:" << MI);
}

unsigned AquilaRegisterInfo::
getFrameRegister(const MachineFunction &MF) const {
	return Aquila::R4;
}
