CC=gcc
EXE1 = seg-perf
SRC=src/
INC= includes/
LIB=lib/

all: $(EXE1)

$(EXE1): $(SRC)$(EXE1).c $(SRC)algoritmos.c
	$(CC) -g -o $(EXE1) $^
clean:
	rm -f $(EXE1)
