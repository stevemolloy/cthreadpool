CFLAGS = -O0 -Wall -Wextra -Wpedantic -std=c99 -ggdb
CLIBS =

SRC = src
OBJ = obj
BIN = bin

EXE = $(BIN)/example

SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

all: $(EXE)

$(EXE): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CLIBS) $^ -o $@

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN):
	@mkdir -p $(BIN)

$(OBJ):
	@mkdir -p $(OBJ)

run: $(EXE)
	./$(EXE)

clean:
	rm -rf bin obj

