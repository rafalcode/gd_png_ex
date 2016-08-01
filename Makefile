CC=gcc
CFLAGS=-g -Wall
SPECLIBS=-lpng
SPECLIBS2=-lgd -lm

EXES=splim cotten imtile tilim exap chapng seepx olpng olpng_d pn4

# Farily well made image splitter, for use with metapixel ..uses libgd
splim: splim.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS2}

cotten: cotten.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

# change png: simple - if any anything -proof of concept for 
# based on cottenceau's code
# trying to localize the variable: global variables: bad!
chapng: chapng.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

# outlines in pngs: prepareation for eigenimages
olpng: olpng.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}
olpng_d: olpng.c
	${CC} ${CFLAGS} -DDBG -o $@ $^ ${SPECLIBS}

# and behold Istart doing what I've alreay done.
pn4: pn4.c
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
