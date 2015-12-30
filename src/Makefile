PROGRAM=leml

.PHONY: default

default:
	@make -C frontend
	ln -sf frontend/$(PROGRAM) $(PROGRAM)

debug:
	@make -C frontend debug
	ln -sf frontend/$(PROGRAM) $(PROGRAM)

clean:
	@make -C frontend clean
	rm -f $(PROGRAM)
