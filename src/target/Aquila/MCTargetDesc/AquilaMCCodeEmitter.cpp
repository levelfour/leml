#define DEBUG_TYPE "mccodeemitter"
#include "MCTargetDesc/AquilaBaseInfo.h"
#include "MCTargetDesc/AquilaFixupKinds.h"
#include "MCTargetDesc/AquilaMCTargetDesc.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
	class AquilaMCCodeEmitter : public MCCodeEmitter {
		AquilaMCCodeEmitter(const AquilaMCCodeEmitter &); // DO NOT IMPLEMENT
		void operator=(const AquilaMCCodeEmitter &); // DO NOT IMPLEMENT
		const MCInstrInfo &MCII;
		const MCSubtargetInfo &STI;
		MCContext &Ctx;

		public:
		AquilaMCCodeEmitter(const MCInstrInfo &mcii, const MCSubtargetInfo &sti,
				MCContext &ctx) :
			MCII(mcii), STI(sti) , Ctx(ctx) {}

		~AquilaMCCodeEmitter() {}

		// EncodeInstruction - AsmStreamerから実行される
		// 命令をバイナリにして出力する
		void EncodeInstruction(const MCInst &MI, raw_ostream &OS,
				SmallVectorImpl<MCFixup> &Fixups) const;

		private:
		// getBinaryCodeForInstr - TableGenが自動生成
		// 命令のバイナリエンコーディングを取得
		uint64_t getBinaryCodeForInstr(const MCInst &MI,
				SmallVectorImpl<MCFixup> &Fixups) const;

		// getMachineOpValue - TableGenの中から必ず参照される
		// オペランドのバイナリエンコーディングを取得
		unsigned getMachineOpValue(const MCInst &MI,const MCOperand &MO,
				SmallVectorImpl<MCFixup> &Fixups) const;

		// getMemEncoding - TableGenのDecoderMethodで指定
		// load/storeのオペランドのバイナリエンコーディングを取得
		unsigned getMemEncoding(const MCInst &MI, unsigned OpNo,
				SmallVectorImpl<MCFixup> &Fixups) const;

		// getMoveTargetOpValue - TableGenのDecoderMethodで指定
		// moveのオペランドのバイナリエンコーディングを取得
		unsigned getMoveTargetOpValue(const MCInst &MI, unsigned OpNo,
				SmallVectorImpl<MCFixup> &Fixups) const;

		// call命令のオペランドのバイナリエンコーディングを取得
		unsigned getCallTargetOpValue(const MCInst &MI, unsigned OpNo,
				SmallVectorImpl<MCFixup> &Fixups) const;


	}; // class AquilaMCCodeEmitter
}  // namespace

MCCodeEmitter *llvm::createAquilaMCCodeEmitter(const MCInstrInfo &MCII,
		const MCRegisterInfo &MRI,
		const MCSubtargetInfo &STI,
		MCContext &Ctx)
{
	return new AquilaMCCodeEmitter(MCII, STI, Ctx);
}

/// EncodeInstruction - Emit the instruction.
void AquilaMCCodeEmitter::
EncodeInstruction(const MCInst &MI, raw_ostream &OS,
		SmallVectorImpl<MCFixup> &Fixups) const
{
	uint32_t Binary = getBinaryCodeForInstr(MI, Fixups);

	// For now all instructions are 4 bytes
	int Size = 4; // FIXME: Have Desc.getSize() return the correct value!

	for (int i = Size - 1; i >= 0; --i) {
		unsigned Shift = i * 8;
		OS << char((Binary >> Shift) & 0xff);
	}
}

/// getMachineOpValue - Return binary encoding of operand. If the machine
/// operand requires relocation, record the relocation and return zero.
unsigned AquilaMCCodeEmitter::
getMachineOpValue(const MCInst &MI, const MCOperand &MO,
		SmallVectorImpl<MCFixup> &Fixups) const {
	if (MO.isReg()) {
		unsigned Reg = MO.getReg();
		unsigned RegNo = getAquilaRegisterNumbering(Reg);
		return RegNo;
	} else if (MO.isImm()) {
		return static_cast<unsigned>(MO.getImm());
	} else if (MO.isFPImm()) {
		return static_cast<unsigned>(APFloat(MO.getFPImm())
				.bitcastToAPInt().getHiBits(32).getLimitedValue());
	} 

	// MO must be an Expr.
	assert(MO.isExpr());

	llvm_unreachable("not implemented");
	return 0;
}

/// getMemEncoding - Return binary encoding of memory related operand.
/// If the offset operand requires relocation, record the relocation.
unsigned AquilaMCCodeEmitter::
getMemEncoding(const MCInst &MI, unsigned OpNo,
		SmallVectorImpl<MCFixup> &Fixups) const {
	//llvm_unreachable("not implemented");
	// Base register is encoded in bits 19-16, offset is encoded in bits 15-0.
	assert(MI.getOperand(OpNo).isReg());
	unsigned RegBits = getMachineOpValue(MI, MI.getOperand(OpNo),Fixups) << 16;
	unsigned OffBits = getMachineOpValue(MI, MI.getOperand(OpNo+1), Fixups);

	return (OffBits & 0xFFFF) | RegBits;
}

/// getMoveTargetOpValue - Return binary encoding of the move
/// target operand.
unsigned AquilaMCCodeEmitter::
getMoveTargetOpValue(const MCInst &MI, unsigned OpNo,
		SmallVectorImpl<MCFixup> &Fixups) const {
	assert(MI.getOperand(OpNo).isImm());
	unsigned value = getMachineOpValue(MI, MI.getOperand(OpNo),Fixups);
	return value & 0xFFFFF;
}

/// getJumpTargetOpValue - Return binary encoding of the jump
/// target operand. If the machine operand requires relocation,
/// record the relocation and return zero.
unsigned AquilaMCCodeEmitter::
getCallTargetOpValue(const MCInst &MI, unsigned OpNo,
		SmallVectorImpl<MCFixup> &Fixups) const {

	const MCOperand &MO = MI.getOperand(OpNo);
	assert(MO.isExpr() && "getCallTargetOpValue expects only expressions");

	const MCExpr *Expr = MO.getExpr();
	Fixups.push_back(MCFixup::Create(0, Expr,
				MCFixupKind(Aquila::fixup_Aquila_24)));
	return 0;
}

#include "AquilaGenMCCodeEmitter.inc"
