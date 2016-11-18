CC=gcc
EXE1 = seg-perf
EXE2 = desECB
EXE3 = desNLTest
EXE4 = desAvTest
EXE5 = aesNLTest
EXE6 = SBOX_AES
EXE7 = AesCBC
EXE8 = desCBC
SRC=src/
INC= includes/
LIB=lib/

all: $(EXE1) $(EXE2) $(EXE3) $(EXE4) $(EXE5) $(EXE6) $(EXE7) $(EXE8)

$(EXE1): $(SRC)$(EXE1).c $(SRC)algoritmos.c $(INC)algoritmos.h
	$(CC) --std=c99 -g -Wall -o $@ $^

$(EXE2): $(SRC)$(EXE2).c $(SRC)algoritmos.c $(INC)algoritmos.h $(SRC)operations_bits.c $(INC)operations_bits.h $(SRC)des.c $(INC)des.h
	$(CC) --std=c99 -g -Wall -o $(EXE2) $^ -lm

$(EXE3): $(SRC)nonlinearityDESTest.c
	$(CC) --std=c99 -g -Wall -o $@ $^

$(EXE4): $(SRC)desAvalancheTest.c $(SRC)algoritmos.c $(INC)algoritmos.h $(SRC)operations_bits.c $(INC)operations_bits.h $(SRC)des.c $(INC)des.h
	$(CC) --std=c99 -g -Wall -o $@ $^ -lm

$(EXE5): $(SRC)nonlinearityAESTest.c
	$(CC) --std=c99 -g -Wall -o $@ $^

$(EXE6): $(SRC)$(EXE6).c $(SRC)algoritmos.c $(INC)algoritmos.h
	$(CC) --std=c99 -g -Wall -o $@ $^

$(EXE7): $(SRC)$(EXE7).c $(SRC)algoritmos.c $(INC)algoritmos.h $(SRC)aes.c $(INC)aes.h
	$(CC) --std=c99 -g -Wall -o $@ $^

$(EXE8): $(SRC)$(EXE8).c $(SRC)algoritmos.c $(INC)algoritmos.h $(SRC)operations_bits.c $(INC)operations_bits.h $(SRC)des.c $(INC)des.h
	$(CC) --std=c99 -g -Wall -o $(EXE8) $^ -lm

clean:
	rm -f $(EXE1) $(EXE2) $(EXE3) $(EXE4) $(EXE5) $(EXE6) $(EXE7) $(EXE8)
