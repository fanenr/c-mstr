LDFLAGS = -g -lasan
NOWARN  = -Wno-unused-variable -Wno-unused-function
CFLAGS  = -Wall -Wextra $(NOWARN) -ggdb3 -std=gnu11 -fsanitize=address
export LDFLAGS CFLAGS

targets := mstr
objects := $(targets:%=%.o)
export targets objects

.PHONY: all
all: $(objects)

$(objects): %.o: %.c %.h
	gcc $(CFLAGS) -c $< 

.PHONY: test run
test: $(objects)
	cd tests && make

run: $(objects)
	cd tests && make run

.PHONY: json
json:
	make clean && bear -- make test

.PHONY: clean
clean:
	-rm -f *.o && cd tests && make clean
