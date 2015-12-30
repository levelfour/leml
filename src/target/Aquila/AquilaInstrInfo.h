#ifndef AQUILA_INSTRUCTIONINFO_H
#define AQUILA_INSTRUCTIONINFO_H

#include "Aquila.h"
#include "AquilaRegisterInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "AquilaGenInstrInfo.inc"

namespace llvm {

	namespace Aquila {
		/// GetOppositeBranchOpc - Return the inverse of the specified
		/// opcode, e.g. turning BEQ to BNE.
		unsigned GetOppositeBranchOpc(unsigned Opc);
	}

	class AquilaInstrInfo : public AquilaGenInstrInfo {
		AquilaTargetMachine &TM;
		const AquilaRegisterInfo RI;
		public:
		explicit AquilaInstrInfo(AquilaTargetMachine &TM);

		/// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
		/// such, whenever a client has an instance of instruction info, it should
		/// always be able to get register info as well (through this method).
		///
		virtual const AquilaRegisterInfo &getRegisterInfo() const;

		/// isLoadFromStackSlot - If the specified machine instruction is a direct
		/// load from a stack slot, return the virtual or physical register number of
		/// the destination along with the FrameIndex of the loaded stack slot.  If
		/// not, return 0.  This predicate must return 0 if the instruction has
		/// any side effects other than loading from the stack slot.
		virtual unsigned isLoadFromStackSlot(const MachineInstr *MI,
				int &FrameIndex) const;

		/// isStoreToStackSlot - If the specified machine instruction is a direct
		/// store to a stack slot, return the virtual or physical register number of
		/// the source reg along with the FrameIndex of the loaded stack slot.  If
		/// not, return 0.  This predicate must return 0 if the instruction has
		/// any side effects other than storing to the stack slot.
		virtual unsigned isStoreToStackSlot(const MachineInstr *MI,
				int &FrameIndex) const;

		virtual void copyPhysReg(MachineBasicBlock &MBB,
				MachineBasicBlock::iterator MI, DebugLoc DL,
				unsigned DestReg, unsigned SrcReg,
				bool KillSrc) const;
		virtual void storeRegToStackSlot(MachineBasicBlock &MBB,
				MachineBasicBlock::iterator MBBI,
				unsigned SrcReg, bool isKill, int FrameIndex,
				const TargetRegisterClass *RC,
				const TargetRegisterInfo *TRI) const;

		virtual void loadRegFromStackSlot(MachineBasicBlock &MBB,
				MachineBasicBlock::iterator MBBI,
				unsigned DestReg, int FrameIndex,
				const TargetRegisterClass *RC,
				const TargetRegisterInfo *TRI) const;

		/// Branch Analysis
		virtual bool AnalyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
				MachineBasicBlock *&FBB,
				SmallVectorImpl<MachineOperand> &Cond,
				bool AllowModify) const;

		virtual unsigned RemoveBranch(MachineBasicBlock &MBB) const;

		virtual unsigned InsertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
				MachineBasicBlock *FBB,
				const SmallVectorImpl<MachineOperand> &Cond,
				DebugLoc DL) const;
	};

}

#endif
