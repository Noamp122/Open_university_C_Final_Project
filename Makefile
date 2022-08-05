
assembler: main.o
	gcc -g3 -ansi -Wall -pedantic  main.o -lm -o assembler

main.o: main.c main.h handler.h in_out_put.h finals.h symbol.h files_handler.h is_register.h
	gcc  -g3 -ansi -Wall -Wextra  -Werror -pedantic -c main.c

clean:
	rm *.o assembler
.PHONY: clean
