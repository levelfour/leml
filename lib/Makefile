PROGRAM=leml

.PHONY: default

default:
	@make -C Frontend
	@make -C External
	ln -sf Frontend/$(PROGRAM) $(PROGRAM)

debug:
	@make -C Frontend debug
	@make -C External
	ln -sf Frontend/$(PROGRAM) $(PROGRAM)

clean:
	@make -C Frontend clean
	@make -C External clean
	rm -f $(PROGRAM)
