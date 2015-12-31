#ifndef AQUILAMCTARGETDESC_H
#define AQUILAMCTARGETDESC_H

#include "llvm/Support/DataTypes.h"
#include "llvm/Support/Debug.h"

namespace llvm {
	class MCAsmBackend;
	class MCCodeEmitter;
	class MCContext;
	class MCInstrInfo;
	class MCObjectWriter;
	class MCRegisterInfo;
	class MCSubtargetInfo;
	class StringRef;
	class Target;
	class raw_ostream;

	extern Target TheAquilaTarget;

	MCCodeEmitter *createAquilaMCCodeEmitter(const MCInstrInfo &MCII,
			const MCRegisterInfo &MRI,
			const MCSubtargetInfo &STI,
			MCContext &Ctx);

	MCAsmBackend *createAquilaAsmBackend(const Target &T, StringRef TT, StringRef CPU);

	MCObjectWriter *createAquilaELFObjectWriter(raw_ostream &OS,
			uint8_t OSABI);
} // End llvm namespace

// Defines symbolic names for Aquila registers.  This defines a mapping from
// register name to register number.
#define GET_REGINFO_ENUM
#include "AquilaGenRegisterInfo.inc"

// Defines symbolic names for the Aquila instructions.
#define GET_INSTRINFO_ENUM
#include "AquilaGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "AquilaGenSubtargetInfo.inc"

#endif
