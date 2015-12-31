#ifndef TARGET_AQUILA_H
#define TARGET_AQUILA_H

#include "MCTargetDesc/AquilaMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Format.h"

namespace llvm {
  class AquilaTargetMachine;
  class FunctionPass;

  FunctionPass *createAquilaISelDag(AquilaTargetMachine &TM);
} // end namespace llvm;

#endif
