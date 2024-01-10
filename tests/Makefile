LDFLAGS   = -g
MAKEFLAGS = --no-print-directory
CFLAGS    = -Wall -Wextra -Werror -ggdb -std=gnu11

tests := test_mstr

.PHONY: all
all: $(tests)

$(tests): test_%: %
	./$<
	@echo "$< test passed"

mstr: mstr.o ../mstr.o
	gcc -o $@ $^ $(LDFLAGS)

mstr.o: mstr.c ../mstr.h
	gcc -c $< $(CFLAGS)

../mstr.o: ../mstr.c ../mstr.h
	cd .. && make

clean:
	-rm -f *.o $(tests:test_%=%)
