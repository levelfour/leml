#ifndef AQUILA_TARGETASMINFO_H
#define AQUILA_TARGETASMINFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {
	class StringRef;
	class Target;

	class AquilaMCAsmInfo : public MCAsmInfo {
		virtual void anchor() {};
		public:
		explicit AquilaMCAsmInfo(const Target &T, StringRef TT);
	};

} // namespace llvm

#endif
