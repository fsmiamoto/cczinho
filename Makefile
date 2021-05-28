CC = clang
CFLAGS = -Wall -Werror -static -std=c11

BIN = czinho
SRC = *.c

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $< -o $(BIN)

test: $(BIN)
	./test.sh

clean:
	$(RM) $(BIN) *.o *~ tmp*

.PHONY: test clean
