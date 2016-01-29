CC=gcc
EXTLIBS=__builtins.ll

.PHONY: all
all: $(EXTLIBS)

%.ll: %.c
	$(CC) -emit-llvm -S -O -o $@ $<

.PHONY: clean
clean:
	rm -rf $(EXTLIBS)
