#ifndef AQUILA_SELECTIONDAGINFO_H
#define AQUILA_SELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {

	class AquilaTargetMachine;

	class AquilaSelectionDAGInfo : public TargetSelectionDAGInfo {
		public:
			explicit AquilaSelectionDAGInfo(const AquilaTargetMachine &TM);
			~AquilaSelectionDAGInfo();
	};
} // end of namespace llvm

#endif
