LEVEL = ../../..
LIBRARYNAME = LLVMAquilaCodeGen
TARGET = Aquila

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = AquilaGenRegisterInfo.inc AquilaGenInstrInfo.inc \
                AquilaGenAsmWriter.inc \
                AquilaGenDAGISel.inc AquilaGenCallingConv.inc \
                AquilaGenSubtargetInfo.inc AquilaGenMCCodeEmitter.inc \
                AquilaGenDisassemblerTables.inc

DIRS = InstPrinter Disassembler TargetInfo MCTargetDesc

include $(LEVEL)/Makefile.common
