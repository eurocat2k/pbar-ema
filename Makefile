CC=clang
CFLAGS=-g -O2 -Wall -Wextra -I.
LDFLAGS=-lc

BIN=pbar

all: $(BIN)

%: %.c
	$(CC) $< -o $(BIN) $(LDFLAGS)

clean:
	@rm $(BIN)
