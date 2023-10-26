CC=clang
CFLAGS=-g -O2 -Wall -Wextra -I.
LDFLAGS=-lc

BIN=pbar

all: $(BIN)

%: %.c
# Windows:
#	$(CC) $< -o $(BIN).exe $(LDFLAGS)
# UNIX:
	$(CC) $< -o $(BIN) $(LDFLAGS)
clean:
# Windows:
#	@rm $(BIN).exe
# Unix:
	@rm $(BIN)
