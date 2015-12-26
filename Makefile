PROGRAM=leml

.PHONY: default, clean

default:
	@make -C src
	ln -sf src/$(PROGRAM) $(PROGRAM)

clean:
	@make -C src clean
	rm -f $(PROGRAM)
