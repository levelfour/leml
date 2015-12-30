#define DEBUG_TYPE "asm-printer"
#include "AquilaInstPrinter.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#include "AquilaGenAsmWriter.inc"

void AquilaInstPrinter::
printRegName(raw_ostream &OS, unsigned RegNo) const {
	OS << '$' << StringRef(getRegisterName(RegNo)).lower();
}

void AquilaInstPrinter::
printInst(const MCInst *MI, raw_ostream &O, StringRef Annot) {
	DEBUG(dbgs() << ">>> printInst:"; MI->dump());
	printInstruction(MI, O);
	printAnnotation(O, Annot);
}

void AquilaInstPrinter::
printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
	DEBUG(dbgs() << ">>> printOperand:" << *MI << " OpNo:" << OpNo << "\n");
	const MCOperand &Op = MI->getOperand(OpNo);
	if (Op.isReg()) {
		printRegName(O, Op.getReg());
	} else if (Op.isImm()) {
		O << Op.getImm();
	} else {
		assert(Op.isExpr() && "unknown operand kind in printOperand");
		O << *Op.getExpr();
	}
}

void AquilaInstPrinter::
printMemOperand(const MCInst *MI, int opNum, raw_ostream &O) {
	DEBUG(dbgs() << ">>> printMemOperand:"; MI->dump());
	printOperand(MI, opNum+1, O);
	O << "(";
	printOperand(MI, opNum, O);
	O << ")";
}
