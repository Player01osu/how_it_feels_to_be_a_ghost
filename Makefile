CC := gcc
IN := src/*.c
OUT := target/main
CFLAGS := -Wall -pedantic -Wno-deprecated-declarations -ffast-math -fomit-frame-pointer
BENCHMARK := -O2
DEBUG_FLAGS := -DDEBUG -ggdb

all:
	${CC} ${IN} ${CFLAGS} -o ${OUT}

benchmark:
	${CC} ${IN} ${CFLAGS} ${BENCHMARK} -o ${OUT}

debug:
	${CC} ${IN} ${CFLAGS} ${DEBUG_FLAGS} -o ${OUT}

clean:
	rm -f target/*
