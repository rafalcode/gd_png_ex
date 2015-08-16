CC=gcc
CFLAGS=-g -Wall
SPECLIBS=-lpng
SPECLIBS2=-lgd -lm
<<<<<<< HEAD
EXES=splim cotten imtile tilim exap chapng
=======
EXES=splim cotten imtile tilim exap seepx
>>>>>>> aadde24db1db4e6847a739dcccf1c46f9483bc9b

# Farily well made image splitter, for use with metapixel ..uses libgd
splim: splim.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS2}

cotten: cotten.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

<<<<<<< HEAD
# change png: simple - if any anything -proof of concept for 
# based on cottenceau's code
# trying to localize the variable: global variables: bad!
chapng: chapng.c
=======
# "exap": examine png. no writing.
exap: exap.c
>>>>>>> aadde24db1db4e6847a739dcccf1c46f9483bc9b
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
