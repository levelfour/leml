#ifndef AQUILA_MCINSTLOWER_H
#define AQUILA_MCINSTLOWER_H

#include "llvm/Support/Compiler.h"
#include "llvm/CodeGen/MachineOperand.h"

namespace llvm {
	class AsmPrinter;
	class MCContext;
	class MCInst;
	class MCOperand;
	class MCSymbol;
	class MachineInstr;
	class MachineModuleInfoMachO;
	class MachineOperand;
	class Mangler;

	/// AquilaMCInstLower - This class is used to lower an MachineInstr
	/// into an MCInst.
	class LLVM_LIBRARY_VISIBILITY AquilaMCInstLower {
		typedef MachineOperand::MachineOperandType MachineOperandType;
		MCContext &Ctx;
		Mangler &Mang;
		const AsmPrinter &Printer;

		public:
		AquilaMCInstLower(MCContext &ctx, Mangler &mang, AsmPrinter &printer)
			: Ctx(ctx), Mang(mang), Printer(printer) {}
		void Lower(const MachineInstr *MI, MCInst &OutMI) const;

		private:
		MCOperand LowerOperand(const MachineOperand& MO) const;
		MCOperand LowerSymbolOperand(const MachineOperand &MO,
				MachineOperandType MOTy) const;
	};
} // end of namespace llvm

#endif
