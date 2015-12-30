#ifndef AQUILAREGISTERINFO_H
#define AQUILAREGISTERINFO_H

#include "Aquila.h"
#include "llvm/Target/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "AquilaGenRegisterInfo.inc"

namespace llvm {
	class AquilaSubtarget;
	class TargetInstrInfo;
	class Type;

	struct AquilaRegisterInfo : public AquilaGenRegisterInfo {
		const TargetInstrInfo &TII;

		AquilaRegisterInfo(const TargetInstrInfo &tii);

		/// Code Generation virtual methods...
		const uint16_t *getCalleeSavedRegs(const MachineFunction* MF = 0) const /*override*/;
		const uint32_t *getCallPreservedMask(CallingConv::ID) const /*override*/;

		BitVector getReservedRegs(const MachineFunction &MF) const /*override*/;

		void eliminateCallFramePseudoInstr(MachineFunction &MF,
				MachineBasicBlock &MBB,
				MachineBasicBlock::iterator I) const /*override*/;

		/// Stack Frame Processing Methods
		void eliminateFrameIndex(MachineBasicBlock::iterator II,
				int SPAdj, RegScavenger *RS = NULL) const;

		/// Debug information queries.
		unsigned getFrameRegister(const MachineFunction &MF) const /*override*/;
	};

} // end namespace llvm


#endif
