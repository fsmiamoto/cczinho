CC = clang
CFLAGS = -Wall -Wextra -Werror -static -std=c11

BIN = cczinho
SRC = $(wildcard *.c)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $< -o $(BIN)

test: $(BIN)
	@BIN=$(BIN) CC=$(CC) ./test.sh

clean:
	$(RM) $(BIN) *.o *~ tmp*

.PHONY: test clean
