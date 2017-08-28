$(shell mkdir -p bin)


FINAL_PROJECT: bin/assembler.o bin/errors.o bin/cmdtable.o bin/datatable.o bin/saveFiles.o bin/symboltable.o bin/main.o
	gcc  -o "FINAL_PROJECT"  ./bin/assembler.o ./bin/cmdtable.o ./bin/datatable.o ./bin/errors.o ./bin/main.o ./bin/saveFiles.o ./bin/symboltable.o  -lm

bin/assembler.o: src/assembler.c src/errors.h src/assembler.h
	gcc -ansi -pedantic -Wall -c src/assembler.c -o bin/assembler.o

bin/errors.o: src/errors.c src/errors.h src/constants.h
	gcc -ansi -pedantic -Wall -c src/errors.c -o bin/errors.o


bin/cmdtable.o: src/cmdtable.c src/cmdtable.h
	gcc -ansi -pedantic -Wall -c src/cmdtable.c -o bin/cmdtable.o

bin/datatable.o: src/datatable.c src/datatable.h src/constants.h
	gcc -ansi -pedantic -Wall -c src/datatable.c -lm -o bin/datatable.o



bin/saveFiles.o: src/saveFiles.c src/cmdtable.h src/datatable.h src/assembler.h src/symbolstructs.h
	gcc -ansi -pedantic -Wall -c src/saveFiles.c -o bin/saveFiles.o

bin/symboltable.o: src/symboltable.c src/assembler.h src/symboltable.h src/symbolstructs.h
	gcc -ansi -pedantic -Wall -c src/symboltable.c -o bin/symboltable.o

	

bin/main.o: src/main.c src/assembler.h src/symboltable.h src/cmdtable.h src/datatable.h src/saveFiles.h
	gcc -ansi -pedantic -Wall -c src/main.c -o bin/main.o


clean:
	rm -r bin
	rm FINAL_PROJECT
