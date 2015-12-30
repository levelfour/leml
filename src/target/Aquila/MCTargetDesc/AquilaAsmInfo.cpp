#include "AquilaMCAsmInfo.h"
#include "llvm/ADT/StringRef.h"
using namespace llvm;

AquilaMCAsmInfo::AquilaMCAsmInfo(const Target &T, StringRef TT) {
	PointerSize = 4;

	PrivateGlobalPrefix = ".L";
	//WeakRefDirective ="\t.weak\t";
	PCSymbol=".";
	CommentString = ";";

	AlignmentIsInBytes = false;
	AllowNameToStartWithDigit = true;
	UsesELFSectionDirectiveForBSS = true;
}
