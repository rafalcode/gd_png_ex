CC=gcc
CFLAGS=-O3
DCFLAGS=-g -Wall -DDBG
SPECLIBS=-lpng
SPECLIBS2=-lgd -lm

EXES=splim cotten imtile tilim exap chapng seepx olpng olpng_d pn4 kro magpng magpng_d mandpng spng0 setc0 w2bthresh findb fb2 crea fb7p fb3p

# Fairly well made image splitter, for use with metapixel ... WARNING is not libpng ... uses libgd!
splim: splim.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS2}

# there's too few arguments to this:
cotten: cotten.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

# change png: simple - if any anything -proof of concept for 
# based on cottenceau's code
# trying to localize the variable: global variables: bad!
chapng: chapng.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}
# yes, quite bad, doesn't work at all.

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
	${CC} ${DCFLAGS} -o $@ $^ ${SPECLIBS}

# proof of concept on the kronecker/magnifier I'm trying to achieve
kro: kro0.c
	${CC} ${DCFLAGS} -o $@ $^ ${SPECLIBS}

magpng: magpng.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}
magpng_d: magpng.c
	${CC} ${DCFLAGS} -o $@ $^ ${SPECLIBS}

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

# read and print out png values
rdprtn: rdprtn.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

# Courtesy of Andrews Greensted, originally called makePNG.
mandpng: mandpng.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS} -lm

#spng series, these are just simple pngs  of size 16x16 but these can be easily changed.
# creating a png from scratch.
spng0: spng0.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

# setting a certain colour to something.
w2bthresh: w2bthresh.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

setc0: setc0.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}

# find black.
findb: findb.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS}
# variation pick out pur black pixels.
fb2: fb2.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS} -lm
# this one hard coded, only for seven point calcs of front bike frame ratios
fb7p: fb7p.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS} -lm
# gimp is so fiddly, 7 points is too much. Go for three
fb3p: fb3p.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS} -lm
# from chatgpt
crea: crea.c
	${CC} ${CFLAGS} -o $@ $^ ${SPECLIBS} -lm

.PHONY: clean

clean:
	rm -f ${EXES}
