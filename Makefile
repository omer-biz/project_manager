

main: clean
	gcc -o main -lncurses main.c nxjson/nxjson.c
	./main

dbg:
	gcc -g -o main -lncurses main.c nxjson/nxjson.c

default:
	gcc -o demo -lncurses demo.c
	./demo

clean:
	rm -fv demo main

.PHONY: clean
