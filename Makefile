CC = clang
CFLAGS = -Wall -Wextra -Werror -static -std=c11

BIN = cczinho
SRC = $(wildcard *.c)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

$(BIN).debug: $(SRC)
	$(CC) $(CFLAGS) -g $< -o $@

test: $(BIN) # run tests
	@BIN=$(BIN) CC=$(CC) ./test.sh

clean:
	$(RM) $(BIN) *.o *~ tmp* $(BIN).debug

debug: $(BIN).debug
	gdb -tui $(BIN).debug

.PHONY: test clean debug
