include "llvm/Target/Target.td"

// Registers
class AquilaReg<bits<6> num, string n> : Register<n> {
	field bits<6> num;
	let Namespace = "Aquila";
}

// General Purpose Registers
def R0  : AquilaReg< 0, "r0" >, DwarfRegNum<[0]>;
def R1  : AquilaReg< 1, "r1" >, DwarfRegNum<[1]>;
def R2  : AquilaReg< 2, "r2" >, DwarfRegNum<[2]>;
def R3  : AquilaReg< 3, "r3" >, DwarfRegNum<[3]>;
def R4  : AquilaReg< 4, "r4" >, DwarfRegNum<[4]>;
def R5  : AquilaReg< 5, "r5" >, DwarfRegNum<[5]>;
def R6  : AquilaReg< 6, "r6" >, DwarfRegNum<[6]>;
def R7  : AquilaReg< 7, "r7" >, DwarfRegNum<[7]>;
def R8  : AquilaReg< 8, "r8" >, DwarfRegNum<[8]>;
def R9  : AquilaReg< 9, "r9" >, DwarfRegNum<[9]>;
def R10 : AquilaReg<10, "r10">, DwarfRegNum<[10]>;
def R11 : AquilaReg<11, "r11">, DwarfRegNum<[11]>;
def R12 : AquilaReg<12, "r12">, DwarfRegNum<[12]>;
def R13 : AquilaReg<13, "r13">, DwarfRegNum<[13]>;
def R14 : AquilaReg<14, "r14">, DwarfRegNum<[14]>;
def R15 : AquilaReg<15, "r15">, DwarfRegNum<[15]>;
def R16 : AquilaReg<16, "r16">, DwarfRegNum<[16]>;
def R17 : AquilaReg<17, "r17">, DwarfRegNum<[17]>;
def R18 : AquilaReg<18, "r18">, DwarfRegNum<[18]>;
def R19 : AquilaReg<19, "r19">, DwarfRegNum<[19]>;
def R20 : AquilaReg<20, "r20">, DwarfRegNum<[20]>;
def R21 : AquilaReg<21, "r21">, DwarfRegNum<[21]>;
def R22 : AquilaReg<22, "r22">, DwarfRegNum<[22]>;
def R23 : AquilaReg<23, "r23">, DwarfRegNum<[23]>;
def R24 : AquilaReg<24, "r24">, DwarfRegNum<[24]>;
def R25 : AquilaReg<25, "r25">, DwarfRegNum<[25]>;
def R26 : AquilaReg<26, "r26">, DwarfRegNum<[26]>;
def R27 : AquilaReg<27, "r27">, DwarfRegNum<[27]>;
def R28 : AquilaReg<28, "r28">, DwarfRegNum<[28]>;
def R29 : AquilaReg<29, "r29">, DwarfRegNum<[29]>;
def R30 : AquilaReg<30, "r30">, DwarfRegNum<[30]>;
def R31 : AquilaReg<31, "r31">, DwarfRegNum<[31]>;
def F0  : AquilaReg<32, "f0" >, DwarfRegNum<[32]>;
def F1  : AquilaReg<33, "f1" >, DwarfRegNum<[33]>;
def F2  : AquilaReg<34, "f2" >, DwarfRegNum<[34]>;
def F3  : AquilaReg<35, "f3" >, DwarfRegNum<[35]>;
def F4  : AquilaReg<36, "f4" >, DwarfRegNum<[36]>;
def F5  : AquilaReg<37, "f5" >, DwarfRegNum<[37]>;
def F6  : AquilaReg<38, "f6" >, DwarfRegNum<[38]>;
def F7  : AquilaReg<39, "f7" >, DwarfRegNum<[39]>;
def F8  : AquilaReg<40, "f8" >, DwarfRegNum<[40]>;
def F9  : AquilaReg<41, "f9" >, DwarfRegNum<[41]>;
def F10 : AquilaReg<42, "f10">, DwarfRegNum<[42]>;
def F11 : AquilaReg<43, "f11">, DwarfRegNum<[43]>;
def F12 : AquilaReg<44, "f12">, DwarfRegNum<[44]>;
def F13 : AquilaReg<45, "f13">, DwarfRegNum<[45]>;
def F14 : AquilaReg<46, "f14">, DwarfRegNum<[46]>;
def F15 : AquilaReg<47, "f15">, DwarfRegNum<[47]>;
def F16 : AquilaReg<48, "f16">, DwarfRegNum<[48]>;
def F17 : AquilaReg<49, "f17">, DwarfRegNum<[49]>;
def F18 : AquilaReg<50, "f18">, DwarfRegNum<[50]>;
def F19 : AquilaReg<51, "f19">, DwarfRegNum<[51]>;
def F20 : AquilaReg<52, "f20">, DwarfRegNum<[52]>;
def F21 : AquilaReg<53, "f21">, DwarfRegNum<[53]>;
def F22 : AquilaReg<54, "f22">, DwarfRegNum<[54]>;
def F23 : AquilaReg<55, "f23">, DwarfRegNum<[55]>;
def F24 : AquilaReg<56, "f24">, DwarfRegNum<[56]>;
def F25 : AquilaReg<57, "f25">, DwarfRegNum<[57]>;
def F26 : AquilaReg<58, "f26">, DwarfRegNum<[58]>;
def F27 : AquilaReg<59, "f27">, DwarfRegNum<[59]>;
def F28 : AquilaReg<60, "f28">, DwarfRegNum<[60]>;
def F29 : AquilaReg<61, "f29">, DwarfRegNum<[61]>;
def F30 : AquilaReg<62, "f30">, DwarfRegNum<[62]>;
def F31 : AquilaReg<63, "f31">, DwarfRegNum<[63]>;

// Register Class
def CPURegs : RegisterClass<"Aquila", [i32], 32, (add
	// Return value
	R3, F3,
	// Caller-saved
	R8, R9, R10, R11, R12, R13, R14, R15,
	R16, R17, R18, R19,
	F4, F5, F6, F7, F8, F9, F10, F11, F12,
	F13, F14, F15, F16, F17, F18, F19,
	// Callee-saved
	R20, R21, R22, R23, R24, R25, R26, R27,
	R28, R29, R30, R31,
	F20, F21, F22, F23, F24, F25, F26, F27,
	F28, F29, F30, F31,
	// Reserved
	R0, F0,         // zero-register
	R1, R2, F1, F2, // assembly-temporary
	R4,             // return-address
	R5,             // heap-pointer
	R6,             // closure-pointer
	R7              // stack-pointer
	)>;

// Functional Unit
def ALU : FuncUnit;

// Instruction Itinerary
def IICAlu    : InstrItinClass;
def IICLoad   : InstrItinClass;
def IICStore  : InstrItinClass;
def IICBranch : InstrItinClass;
def IICPseudo : InstrItinClass;

// Aquila Generic instruction itinerary
//   here can define cycle-num of each instruction type
def AquilaGenericItineraries : ProcessorItineraries<[ALU], [], [
	InstrItinData<IICAlu,    [InstrStage<1, [ALU]>]>,
	InstrItinData<IICLoad,   [InstrStage<1, [ALU]>]>,
	InstrItinData<IICStore,  [InstrStage<1, [ALU]>]>,
	InstrItinData<IICBranch, [InstrStage<1, [ALU]>]>,
	InstrItinData<IICPseudo, [InstrStage<1, [ALU]>]>
]>;


// load/storeで利用するオペランド.20bit
// 19-16: CPURegs Operand(base)
// 15-0 : 符号付き16bit整数(offset)
// printMethod: base(offset) 形式で出力
// EncoderMethod: bit列から2つの値を取得
def mem : Operand<i32> {
	let PrintMethod = "printMemOperand";
	let MIOperandInfo = (ops CPURegs, i16imm);
	let EncoderMethod = "getMemEncoding";
}

// 即値ロード用のオペランド. 20bit
// 19-0: 符号付き20bit整数
// EncoderMethod: bit列から符号付き20bit整数を取得
def movetarget : Operand<i32> {
	let EncoderMethod = "getMoveTargetOpValue";
}

def calltarget  : Operand<iPTR> {
	let EncoderMethod = "getCallTargetOpValue";
}

// 符号付き20bit整数
// 定数(ISD::Constant)で20bitで表現可能なもの
def immSExt20  : PatLeaf<(imm), [{ return isInt<20>(N->getSExtValue()); }]>;

def addr : ComplexPattern<iPTR, 2, "SelectAddr", [], []>;

// Instruction Format
class Format<bits<3> val> {
	bits<3> Value = val;
}

def Pseudo : Format<0>;
def XForm  : Format<1>;
def BForm  : Format<2>;
def RForm  : Format<3>;
def IForm  : Format<4>;

// Common Instruction Format
class AquilaInst<dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin, Format f> : Instruction {
	field bits<32> Inst;
	Format Form = f;

	bits<6> Opcode = 0;

	let Namespace      = "Aquila";
	let Size           = 4;
	let Inst{31-26}    = Opcode;
	let OutOperandList = outs;
	let InOperandList  = ins;
	let AsmString      = asmstr;
	let Pattern        = pattern;
	let Itinerary      = itin;

	bits<3> FormBits = Form.Value;

	let DecoderNamespace = "Aquila";

	field bits<32> SoftFail = 0;
}

// X-Format
class AquilaInstXForm<bits<6> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin> : AquilaInst<outs, ins, asmstr, pattern, itin, XForm> {
	bits<5> rd;
	bits<5> rs;
	bits<5> rt;
	bits<10> funct;

	let Opcode = op;
	let Inst{25-21} = rd;
	let Inst{20-16} = rs;
	let Inst{15-11} = rt;
	let Inst{10-1} = funct;
}

// B-Format
class AquilaInstBForm<bits<6> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin> : AquilaInst<outs, ins, asmstr, pattern, itin, BForm> {
	bits<5> rd;
	bits<5> rs;
	bits<16> imm;

	let Opcode = op;
	let Inst{25-21} = rd;
	let Inst{20-16} = rs;
	let Inst{15-0} = imm;
}

// R-Format
class AquilaInstRForm<bits<6> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin> : AquilaInst<outs, ins, asmstr, pattern, itin, RForm> {
	bits<5> rd;
	bits<5> rs;
	bits<5> rt;
	bits<10> funct;

	let Opcode = op;
	let Inst{25-21} = rd;
	let Inst{20-16} = rs;
	let Inst{15-11} = rt;
	let Inst{10-1} = funct;
}

// I-Format
class AquilaInstIForm<bits<6> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin> : AquilaInst<outs, ins, asmstr, pattern, itin, IForm> {
	bits<5> rd;
	bits<5> rs;
	bits<16> imm;

	let Opcode = op;
	let Inst{25-21} = rd;
	let Inst{20-16} = rs;
	let Inst{15-0} = imm;
}

// Pseudo-Instruction
class AquilaPseudo<dag outs, dag ins, string asmstr, list<dag> pattern> : AquilaInst<outs, ins, asmstr, pattern, IICPseudo, Pseudo> {
	let isCodeGenOnly = 1;
	let isPseudo = 1;
}

// SDTypeProfileはSDNodeの必要条件を定義する。
// 第一引数: 結果Node数
// 第二引数: オペランドNode数
// 第三引数: 制約条件(SDTypeConstraint)

// SDTCisInt<N>: N番目のオペランドはInt型
// SDTCisVT<N, VT>: N番目のオペランドはVT型

// SDNodeは新しいSDNodeを定義する。
// 第一引数: opcode
// 第二引数: 制約条件(SDTypeProfile)
// 第三引数: SDNodeProperty

// SDNodeProperty
// SDNPCommutative : // 可換
// SDNPAssociative : // 結合法則
// SDNPHasChain    : // R/W chain operand and result
// SDNPOutGlue     : // Write a flag result
// SDNPInGlue      : // Read a flag operand
// SDNPOptInGlue   : // Optionally read a flag operand
// SDNPMayStore    : // May write to memory, sets 'mayStore'.
// SDNPMayLoad     : // May read memory, sets 'mayLoad'.
// SDNPSideEffect  : // Sets 'HasUnmodelledSideEffects'.
// SDNPMemOperand  : // Touches memory, has assoc MemOperand
// SDNPVariadic    : // 可変引数を持つ
// SDNPWantRoot    : // ComplexPattern gets the root of match
// SDNPWantParent  : // ComplexPattern gets the parent

def SDT_AquilaRet          : SDTypeProfile<0, 1, [SDTCisInt<0>]>;
def AquilaRet : SDNode<"AquilaISD::Ret", SDT_AquilaRet, 
	[SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def SDT_AquilaCall      : SDTypeProfile<0, 1, [SDTCisVT<0, iPTR>]>;

def AquilaCall : SDNode<"AquilaISD::Call",SDT_AquilaCall,
	[SDNPHasChain, SDNPOutGlue, SDNPOptInGlue,
	SDNPVariadic]>;

def SDT_AquilaCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>]>;
def SDT_AquilaCallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;

def callseq_start : SDNode<"ISD::CALLSEQ_START", SDT_AquilaCallSeqStart,
	[SDNPHasChain, SDNPSideEffect, SDNPOutGlue]>;
def callseq_end   : SDNode<"ISD::CALLSEQ_END", SDT_AquilaCallSeqEnd,
	[SDNPHasChain, SDNPSideEffect,
	SDNPOptInGlue, SDNPOutGlue]>;

// Instruction Specific Format
class ArithLogicInst<bits<6> op, string asmstr, SDNode OpNode, InstrItinClass itin, RegisterClass RC>
	: AquilaInstXForm<op, (outs RC:$rd), (ins RC:$rs, RC:$rt),
	 !strconcat(asmstr, "\t$rd, $rs, $rt"),
	 [(set RC:$rd, (OpNode RC:$rs, RC:$rt))], itin> {
}

// Load/Store common form
// DecoderMethod: objdumpなどでバイナリから値をデコードする
class FMem<bits<6> op, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin>
: AquilaInstIForm<op, outs, ins, asmstr, pattern, itin> {
	bits<16> addr;
	let Inst{15-0}  = addr;
	let DecoderMethod = "DecodeMem";
}

// Load
let canFoldAsLoad = 1 in
class LoadM<bits<6> op, string asmstr, RegisterClass RC>:
FMem<op, (outs RC:$rd), (ins mem:$addr),
	!strconcat(asmstr, "\t$rd, $addr"),
	[(set RC:$rd, (load addr:$addr))], IICLoad>;

// Store
class StoreM<bits<6> op, string asmstr, RegisterClass RC>:
FMem<op, (outs), (ins RC:$rd, mem:$addr),
	!strconcat(asmstr, "\t$rd, $addr"),
	[(store RC:$rd, addr:$addr)], IICStore>;

// Return
class RetInst<bits<6> op, string asmstr>:
AquilaInstIForm<op, (outs), (ins CPURegs:$rd),
	!strconcat(asmstr, "\t$rd"), [(AquilaRet CPURegs:$rd)], IICBranch> {
		let isBranch=1;
		let isTerminator=1;
		let isBarrier=1;
		let isReturn=1;
	}

// Load Immediate
// DecoderMethod: 即値ロード用のデコード
class LoadI<bits<6> op, string asmstr>:
AquilaInstIForm<op, (outs CPURegs:$rd), (ins movetarget:$imm),
	!strconcat(asmstr, "\t$rd, $imm"), [(set CPURegs:$rd, immSExt20:$imm)], IICLoad> {
		let DecoderMethod = "DecodeMoveTarget";
	}

class Call<bits<6> op, string asmstr>:
AquilaInstIForm<op, (outs), (ins calltarget:$imm, variable_ops),
	!strconcat(asmstr, "\t$imm"), [(AquilaCall imm:$imm)],
	IICBranch> {
		let isCall=1;
		let DecoderMethod = "DecodeCallTarget";
	}

// Aquila Instruction Definition
def LOAD  : LoadM<0x00, "load", CPURegs>;
def STORE : StoreM<0x01, "store", CPURegs>;
def MOVE  : LoadI<0x02, "move">;
//def CALL  : Call<0x03, "call">;
//def RET   : RetInst<0x04, "ret">;
def ADD   : ArithLogicInst<0x05, "add", add, IICAlu, CPURegs>;
def SUB   : ArithLogicInst<0x06, "sub", sub, IICAlu, CPURegs>;

// Calling Convention
def CC_Aquila : CallingConv<[
	// Arguments
	CCIfType<[i32], CCAssignToReg<[
		R8, R9, R10, R11, R12, R13, R14, R15,
		R16, R17, R18, R19]>>,
	CCIfType<[f32], CCAssignToReg<[
		F4, F5, F6, F7, F8, F9, F10, F11, F12,
		F13, F14, F15, F16, F17, F18, F19]>>,
	CCIfType<[i32], CCAssignToStack<4, 4>>,
	CCIfType<[f32], CCAssignToStack<4, 4>>
]>;

def RetCC_Aquila : CallingConv<[
	// Return Value
	CCIfType<[i32], CCAssignToReg<[R3]>>,
	CCIfType<[f32], CCAssignToReg<[F3]>>
]>;

// Callee-Save
def CSR_SingleFloatOnly : CalleeSavedRegs<(add
	R4,
	(sequence "R%u", 31, 20),
	(sequence "F%u", 31, 20))>;

// Target definition
def AquilaInstrInfo : InstrInfo;

def : Processor<"aquila32", AquilaGenericItineraries, []>;

def AquilaAsmWriter : AsmWriter {
	string AsmWriterClassName = "InstPrinter";
	bit isMCAsmWriter = 1;
}

def Aquila : Target {
	let InstructionSet = AquilaInstrInfo;
	let AssemblyWriters = [AquilaAsmWriter];
}
