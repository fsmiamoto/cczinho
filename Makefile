CC = gcc
CFLAGS = -Wall -Wextra -Werror -static -std=c11

BINDIR = ./bin
BIN = cczinho

SRCDIR = ./src
SRC = $(wildcard $(SRCDIR)/*.c)

VERBOSE ?= 0

$(BINDIR)/$(BIN): $(SRC) $(BINDIR)
	$(CC) $(CFLAGS) $(SRC) -o $@

$(BINDIR)/$(BIN).debug: $(SRC) $(BINDIR)
	$(CC) $(CFLAGS) -g $(SRC) -o $@

$(BINDIR):
	@mkdir -p $(BINDIR)

test: $(BINDIR)/$(BIN) # run tests
	@BIN=$(BINDIR)/$(BIN) CC=$(CC) VERBOSE=$(VERBOSE) ./test.sh

clean:
	$(RM) -rf $(BINDIR) *.o *~ tmp*

debug: $(BINDIR)/$(BIN).debug
	gdb -tui $(BINDIR)/$(BIN).debug

.PHONY: test clean debug
