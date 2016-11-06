CC=gcc
EXE1 = seg-perf
EXE2 = desECB
EXE3 = desNLTest
EXE4 = aesNLTest
SRC=src/
INC= includes/
LIB=lib/

all: $(EXE1) $(EXE2) $(EXE3) $(EXE4)

$(EXE1): $(SRC)$(EXE1).c $(SRC)algoritmos.c $(INC)algoritmos.h
	$(CC) --std=c99 -g -o $@ $^

$(EXE2): $(SRC)$(EXE2).c $(SRC)algoritmos.c $(INC)algoritmos.h
	$(CC) --std=c99 -g -o $@ $^ -lm

$(EXE3): $(SRC)nonlinearityDESTest.c
	$(CC) --std=c99 -g -o $@ $^

$(EXE4): $(SRC)nonlinearityAESTest.c
	$(CC) --std=c99 -g -o $@ $^

clean:
	rm -f $(EXE1) $(EXE2) $(EXE3) $(EXE4)
