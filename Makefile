CC = clang
CFLAGS = -Wall -Wextra -Werror -static -std=c11

BIN = cczinho
SRC = $(wildcard *.c)

%.debug: $(SRC)
	$(CC) $(CFLAGS) -g $< -o $@

%: $(SRC)
	$(CC) $(CFLAGS) $< -o $(BIN)

test: $(BIN)
	@BIN=$(BIN) CC=$(CC) ./test.sh

clean:
	$(RM) $(BIN) *.o *~ tmp*

debug: $(BIN).debug
	gdb -tui "$(BIN).debug"

.PHONY: test clean
