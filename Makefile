CC=gcc
CFLAGS=-g -Wall
SPECLIBS=-lpng
SPECLIBS2=-lgd -lm

EXES=splim cotten imtile tilim exap chapng seepx olpng olpng_d pn4 kro magn

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

chapng_d: chapng.c
	${CC} ${CFLAGS} -DDBG -o $@ $^ ${SPECLIBS}

# Only one that worked: however, there are vlagrind errors ont he interlacing part ... on the update png
exap: exap.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}
# Notes on exap:
# this seems to imply that there are now getters and setters to be used, as in:
# png_get_image_width(png_sptr, info_ptr);
# i.e. you can't assign directly from info_ptr like what I seem to be doing in olpng, etc.

# outlines in pngs: preparation for eigenimages
# it looks as if I dealt with this quite alot.
# forget olpng0.c it seems to be too alpha.
olpng: olpng.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}
olpng_d: olpng.c
	${CC} ${CFLAGS} -DDBG -o $@ $^ ${SPECLIBS}

# proof of concept on the kronecker/magnifier I'm trying to achieve
kro: kro0.c
	${CC} ${CFLAGS} -DDBG -o $@ $^ ${SPECLIBS}

magn: magn.c
	${CC} ${CFLAGS} -DDBG -o $@ $^ ${SPECLIBS}

# and behold Istart doing what I've alreay done.
# and behold Istart doing what I've alreay done.
pn4: pn4.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

# "seepx": examine png. no writing.
# this is not based on cottenceau's code ... adifferent template was used, more complicated.
seepx: seepx.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

imtile: imtile.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS2}

tilim: tilim.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS2}

.PHONY: clean

clean:
	rm -f ${EXES}
