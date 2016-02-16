PROGRAM=leml
RT_DIR=min-rt
RT=$(RT_DIR)/raytracer
SLD=$(RT_DIR)/contest.sld
PPM=output.ppm

include Makefile.in

.PHONY: default, debug, run, clean, cleanall

default:
	@make -C src
	ln -sf src/$(PROGRAM) $(PROGRAM)

debug:
	@make -C src debug
	ln -sf src/$(PROGRAM) $(PROGRAM)

run: $(SLD) $(RT)
	cat $(SLD) | ./$(RT) > $(PPM)

clean:
	rm -f $(RT) $(RT).ll

cleanall:
	@make -C src clean
	rm -f $(PROGRAM) $(RT) $(RT).ll

$(RT): $(PROGRAM) $(RT).ml
	./$(PROGRAM) $(RT).ml -mem2reg -o $(RT).ll
	$(CC) -lm -o $(RT) $(RT).ll
