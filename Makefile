SRC = src/argp_config.c src/main.c src/project.c src/nxjson/nxjson.c
OBJ = ${SRC:.c=.o}
 
pm: ${OBJ} 
	cc -O3 -o $@ ${OBJ}
	strip pm

dbg: ${OBJ}
	cc -g -o $@ ${OBJ}
	gdb -q ./dbg

tui: ${OBJ}
	cc -lncurses -o pm ${OBJ}

.PHONY: clean
clean:
	rm -fv pm dbg ${OBJ}

