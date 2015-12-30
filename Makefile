PROGRAM=leml

.PHONY: default, debug, clean

default:
	@make -C src
	ln -sf src/$(PROGRAM) $(PROGRAM)

debug:
	@make -C src debug
	ln -sf src/$(PROGRAM) $(PROGRAM)

clean:
	@make -C src clean
	rm -f $(PROGRAM)
