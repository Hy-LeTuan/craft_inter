main: main.o chunk.o memory.o debug.o value.o vm.o compiler.o scanner.o
	gcc main.o chunk.o memory.o debug.o value.o vm.o compiler.o scanner.o -o main

main.o: main.c chunk.h debug.h vm.h
	gcc -c main.c

chunk.o: chunk.c chunk.h memory.h common.h value.h
	gcc -c chunk.c

memory.o: memory.c memory.h common.h
	gcc -c memory.c

debug.o: debug.c debug.h chunk.h
	gcc -c debug.c

value.o: value.c value.h memory.h common.h
	gcc -c value.c

vm.o: vm.c vm.h chunk.h common.h value.h debug.h compiler.h
	gcc -c vm.c

compiler.o: compiler.c compiler.h scanner.h
	gcc -c compiler.c

scanner.o: scanner.c scanner.h
	gcc -c scanner.c
