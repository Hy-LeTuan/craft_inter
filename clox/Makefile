main: main.o chunk.o memory.o debug.o value.o vm.o compiler.o scanner.o object.o
	gcc -g main.o chunk.o memory.o debug.o value.o vm.o compiler.o scanner.o object.o -o main

main.o: main.c chunk.h debug.h vm.h
	gcc -g -Wall -c main.c

chunk.o: chunk.c chunk.h memory.h common.h value.h
	gcc -g -Wall -c chunk.c

memory.o: memory.c memory.h common.h vm.h
	gcc -g -Wall -c memory.c

debug.o: debug.c debug.h chunk.h
	gcc -g -Wall -c debug.c

value.o: value.c value.h memory.h common.h object.h
	gcc -g -Wall -c value.c

vm.o: vm.c vm.h chunk.h common.h value.h debug.h compiler.h memory.h table.h
	gcc -g -Wall -c vm.c

compiler.o: compiler.c compiler.h scanner.h debug.h object.h
	gcc -g -Wall -c compiler.c

scanner.o: scanner.c scanner.h
	gcc -g -Wall -c scanner.c

object.o: object.c object.h memory.h value.h table.h
	gcc -g -Wall -c object.c

table.o: table.c table.h memory.h object.h table.h value.h
	gcc -g -Wall -c table.c

clean:
	rm -f *.o main
