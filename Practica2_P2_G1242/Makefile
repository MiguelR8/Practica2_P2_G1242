CC=gcc
EXE1 = seg-perf
EXE2 = desECB
SRC=src/
INC= includes/
LIB=lib/

all: $(EXE1) $(EXE2)

$(EXE1): $(SRC)$(EXE1).c $(SRC)algoritmos.c
	$(CC) -g -o $(EXE1) $^

$(EXE2): $(SRC)$(EXE2).c $(SRC)algoritmos.c
	$(CC) -g -o $(EXE2) $^

clean:
	rm -f $(EXE1) $(EXE2)