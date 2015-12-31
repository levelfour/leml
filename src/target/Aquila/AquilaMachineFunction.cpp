#include "AquilaMachineFunction.h"
#include "AquilaInstrInfo.h"
#include "MCTargetDesc/AquilaMCTargetDesc.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

void AquilaMachineFunctionInfo::anchor() { }
