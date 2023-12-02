tests := $(targets:%=test_%)

.PHONY: all $(tests)
all: $(tests)

$(tests): test_%: %
	./$<
	@echo "$< test passed"

$(targets): %: %.o ../%.o
	gcc -o $@ $^ $(LDFLAGS)

$(objects): %.o: %.c ../%.h
	gcc -c $< $(CFLAGS)

$(objects:%=../%): ../%.o: ../%.c ../%.h
	cd .. && make $(subst ../,,$@)

clean:
	-rm -f *.o $(targets)
