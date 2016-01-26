PROGRAM=leml

# tools
LEX=flex
YACC=bison
CXX=g++
LLVMCONFIG=llvm-config

# options
WARNING_OPTIONS=-Wall -Wno-deprecated-register -Wno-unused-function -Wno-unneeded-internal-declaration -Wno-switch
CXXFLAGS=-std=c++11 -ferror-limit=3 `$(LLVMCONFIG) --cxxflags` $(WARNING_OPTIONS)
DEBUG_FLAGS=-g -O0 -ggdb -DLEML_DEBUG=1 #-DYYDEBUG=1 
LDFLAGS=`$(LLVMCONFIG) --ldflags` -lncurses -lz
LIBS=`$(LLVMCONFIG) --libs all`

# generated
OBJS= main.o \
	  parser.o \
	  lexer.o \
	  syntax.o \
	  codeGen.o \
	  type.o \
	  infer.o \
	  cmdopt.o
LEXTAB=lexer.cc
YACCTAB=parser.cc
YACCFILES=parser.hh

.PHONY: all
all: $(PROGRAM)

.PHONY: debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LIBS)

$(LEXTAB): scanner.l
	$(LEX) -o $@ $<

$(YACCTAB): driver.y
	$(YACC) -d -o $@ $<

%.o: %.cc
	$(CXX) -c $(CXXFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -rf *.dSYM \
		$(PROGRAM) $(OBJS) $(LEXTAB) $(YACCTAB) $(YACCFILES)
