#include "AquilaSubtarget.h"
#include "Aquila.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "AquilaGenSubtargetInfo.inc"

using namespace llvm;

AquilaSubtarget::AquilaSubtarget(const std::string &TT,
		const std::string &CPU,
		const std::string &FS)
: AquilaGenSubtargetInfo(TT, CPU, FS) {
	std::string CPUName = "generic";

	// Parse features string.
	ParseSubtargetFeatures(CPUName, FS);
}
