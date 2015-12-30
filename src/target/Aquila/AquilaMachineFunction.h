#ifndef AQUILA_MACHINE_FUNCTION_INFO_H
#define AQUILA_MACHINE_FUNCTION_INFO_H

#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include <utility>

namespace llvm {

	/// AquilaFunctionInfo - This class is derived from MachineFunction private
	/// Aquila target-specific information for each MachineFunction.
	class AquilaMachineFunctionInfo : public MachineFunctionInfo {
		virtual void anchor();

		public:
		AquilaMachineFunctionInfo(MachineFunction& MF) {}
	};
} // end of namespace llvm

#endif // AQUILA_MACHINE_FUNCTION_INFO_H
