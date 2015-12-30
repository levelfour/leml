#define DEBUG_TYPE "aquila-selectiondag-info"
#include "AquilaTargetMachine.h"
using namespace llvm;

AquilaSelectionDAGInfo::AquilaSelectionDAGInfo(const AquilaTargetMachine &TM)
	: TargetSelectionDAGInfo(TM) {}

AquilaSelectionDAGInfo::~AquilaSelectionDAGInfo() {}
