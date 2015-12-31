#ifndef AQUILA_FRAMELOWERING_H
#define AQUILA_FRAMELOWERING_H

#include "Aquila.h"
#include "AquilaSubtarget.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
	class AquilaSubtarget;

	class AquilaFrameLowering : public TargetFrameLowering {
		protected:
			const AquilaSubtarget &STI;

		public:
			explicit AquilaFrameLowering(const AquilaSubtarget &sti)
				: TargetFrameLowering(StackGrowsDown, 8, 0), STI(sti) {
				}

			/// emitProlog/emitEpilog - These methods insert prolog and epilog code into
			/// the function.
			void emitPrologue(MachineFunction &MF) const /*override*/;
			void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const /*override*/;
			bool hasFP(const MachineFunction &MF) const /*override*/;
	};
} // End llvm namespace

#endif
