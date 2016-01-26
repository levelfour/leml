PROGRAM=leml

.PHONY: default, debug, clean

default:
	@make -C lib
	ln -sf lib/$(PROGRAM) $(PROGRAM)

debug:
	@make -C lib debug
	ln -sf lib/$(PROGRAM) $(PROGRAM)

clean:
	@make -C lib clean
	rm -f $(PROGRAM)
