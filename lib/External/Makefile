EXTLIBS=__builtins.ll

include ../../Makefile.in

.PHONY: all
all: $(EXTLIBS)

%.ll: %.c
	$(CC) -emit-llvm -S -O -o $@ $<

.PHONY: clean
clean:
	rm -rf $(EXTLIBS)
