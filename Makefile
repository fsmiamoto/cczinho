CC = clang
CFLAGS = -Wall -Werror -static -std=c11

BIN = czinho
SRC = *.c

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $< -o $(BIN)

test: $(BIN)
	BIN=$(BIN) CC=$(CC) ./test.sh

clean:
	$(RM) $(BIN) *.o *~ tmp*

.PHONY: test clean
