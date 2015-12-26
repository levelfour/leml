PROGRAM=leml

.PHONY: default, clean

default:
	@make -C src
	ln -s src/$(PROGRAM) $(PROGRAM)

clean:
	@make -C src clean
