CC=gcc
CFLAGS=-g -Wall
SPECLIBS=-lpng
SPECLIBS2=-lgd -lm
EXES=splim cotten imtile tilim exap seepx

# Farily well made image splitter, for use with metapixel ..uses libgd
splim: splim.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS2}

cotten: cotten.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

# "exap": examine png. no writing.
exap: exap.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

# "seepx": examine png. no writing.
seepx: seepx.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

imtile: imtile.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS2}

tilim: tilim.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS2}

.PHONY: clean

clean:
	rm -f ${EXES}
