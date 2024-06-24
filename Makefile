MODE = debug

include config.mk
export LDFLAGS CFLAGS

.PHONY: all
all: test

test: test.o mstr.o
	gcc $(LDFLAGS) -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $<

.PHONY: run
run: test
	./test

.PHONY: json
json: clean
	bear -- make test

.PHONY: clean
clean:
	-rm -f *.o test
