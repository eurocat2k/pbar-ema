SHELL=/bin/sh
CC=clang
CFLAGS=-O2 -Wall -Wextra -I.
CFLAGS_DEBUG=-g -O2 -Wall -Wextra -I. -DDEBUG
LDFLAGS=-lc
SRCS=pbar.c

BIN=pbar
BINDBG=pbar.dbg

all: $(BIN) $(BINDBG)

$(BIN): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(BIN) $(LDFLAGS)
$(BINDBG): $(SRCS)
	$(CC) $(CFLAGS_DEBUG) $(SRCS) -o $(BIN).dbg $(LDFLAGS)
clean:
	@rm -f $(BIN)
	@rm -f $(BIN).core
	@rm -f $(BIN).dbg
	@rm -f $(BIN).dbg.core
