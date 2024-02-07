LDFLAGS   = -g
MAKEFLAGS = --no-print-directory
CFLAGS    = -Wall -Wextra -Werror -ggdb3 -std=gnu11

targets := mstr
objects := $(targets:%=%.o)

.PHONY: all
all: $(objects)

$(objects): %.o: %.c %.h
	gcc -c $< $(CFLAGS)

.PHONY: test
test: $(objects)
	cd tests && make

.PHONY: clean
clean:
	-rm -f *.o
	-cd tests && make clean
