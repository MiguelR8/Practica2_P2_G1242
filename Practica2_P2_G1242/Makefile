CC=gcc
EXE1 = seg-perf
EXE2 = desECB
SRC=src/
INC= includes/
LIB=lib/

all: $(EXE1) $(EXE2)

$(EXE1): $(SRC)$(EXE1).c $(SRC)algoritmos.c $(INC)algoritmos.h
	$(CC) --std=c99 -g -o $(EXE1) $^

$(EXE2): $(SRC)$(EXE2).c $(SRC)algoritmos.c $(INC)algoritmos.h $(SRC)operations_bits.c $(INC)operations_bits.h $(SRC)des.c $(INC)des.h
	$(CC) --std=c99 -g -o $(EXE2) $^ -lm

clean:
	rm -f $(EXE1) $(EXE2)
