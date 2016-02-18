PROGRAM=leml
RT_DIR=min-rt
RT=$(RT_DIR)/raytracer
SLD=$(RT_DIR)/contest.sld
PPM=output.ppm

include Makefile.in

default:
	@make -C src
	ln -sf src/$(PROGRAM) $(PROGRAM)

debug:
	@make -C src debug
	ln -sf src/$(PROGRAM) $(PROGRAM)

.PHONY: run
run: $(SLD) $(RT)
	cat $(SLD) | ./$(RT) > $(PPM)

.PHONY: test
test:
	@sh run_test.sh

.PHONY: clean-rt
clean-rt:
	rm -f $(RT) $(RT).ll

.PHONY: clean
clean:
	@make -C src clean
	rm -f $(PROGRAM) $(RT) $(RT).ll $(PPM)

$(RT): $(PROGRAM) $(RT).ml
	./$(PROGRAM) $(RT).ml -mem2reg -o $(RT).ll
	$(CC) -lm -o $(RT) $(RT).ll
