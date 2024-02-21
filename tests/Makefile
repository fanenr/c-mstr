tasks := $(targets:%=run_%)

.PHONY: all
all: $(targets)

$(targets): %: %.o ../%.o
	gcc $(LDFLAGS) -o $@ $^

$(objects): %.o: %.c ../%.h
	gcc $(CFLAGS) -c $<

$(objects:%=../%): ../%.o: ../%.c ../%.h
	cd .. && make $(subst ../,,$@)

.PHONY: run $(tasks)
run: $(tasks)

$(tasks): run_%: %
	./$<
	@echo "$< test passed"

.PHONY: clean
clean:
	-rm -f *.o $(targets)
