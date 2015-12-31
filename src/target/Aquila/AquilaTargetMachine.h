#ifndef AQUILA_TARGETMACHINE_H
#define AQUILA_TARGETMACHINE_H

#include "AquilaFrameLowering.h"
#include "AquilaInstrInfo.h"
#include "AquilaISelLowering.h"
#include "AquilaSelectionDAGInfo.h"
#include "AquilaRegisterInfo.h"
#include "AquilaSubtarget.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Support/Debug.h"

namespace llvm {

	class Module;

	class AquilaTargetMachine : public LLVMTargetMachine {
		const DataLayout DL;
		AquilaSubtarget Subtarget;
		AquilaInstrInfo InstrInfo;
		AquilaFrameLowering FrameLowering;
		AquilaTargetLowering TLInfo;
		AquilaSelectionDAGInfo TSInfo;

		public:
		AquilaTargetMachine(const Target &T, StringRef TT,
				StringRef CPU, StringRef FS, const TargetOptions &Options,
				Reloc::Model RM, CodeModel::Model CM,
				CodeGenOpt::Level OL);

		virtual const AquilaInstrInfo *getInstrInfo() const {
			return &InstrInfo;
		}
		virtual const AquilaSubtarget *getSubtargetImpl() const {
			return &Subtarget;
		}
		virtual const AquilaRegisterInfo *getRegisterInfo() const {
			return &InstrInfo.getRegisterInfo();
		}
		virtual const DataLayout *getDataLayout() const {
			return &DL;
		}
		virtual const AquilaTargetLowering *getTargetLowering() const {
			return &TLInfo;
		}
		virtual const AquilaFrameLowering *getFrameLowering() const{
			return &FrameLowering;
		}
		virtual const AquilaSelectionDAGInfo* getSelectionDAGInfo() const {
			return &TSInfo;
		}

		// Pass Pipeline Configuration
		virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);
	};
} // end namespace llvm

#endif
