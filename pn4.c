/*
 * Copyright 2002-2008 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 * RFnotes: supposedly this is an easy test of libpng ...
 * but it isn't really , note hte three dots in the first function's arguments: WTF?
 * is that from stdarg.h?
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

/* globals */
int x, y;

typedef struct
{
    int width, height;
    png_byte color_type;
    png_byte bit_depth;
    int npasses;
} dets_t;

/* functs */
void abort_(const char *s, ...)
{
	va_list args; // variable args
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

dets_t *read_png_file(char* file_name, png_structp *pngptr, png_infop *infptr, png_bytep **rptrs)
{
	unsigned char header[8];	// 8 is the maximum size that can be checked: // I put unisgned here .. and get no warnings!

	/* open file and test for it being a png */
	FILE *fp = fopen(file_name, "rb");
	if (!fp)
		abort_("[read_png_file] File %s could not be opened for reading", file_name);
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8)) //differs in signedness warning here! SOLN make header unsigned!
		abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);

	/* initialize stuff */
	*pngptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	if (!*pngptr)
		abort_("[read_png_file] png_create_read_struct failed");

	*infptr = png_create_info_struct(*pngptr);
	if (!*infptr)
		abort_("[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(*pngptr)))
		abort_("[read_png_file] Error during init_io");

	png_init_io(*pngptr, fp);
	png_set_sig_bytes(*pngptr, 8);

	png_read_info(*pngptr, *infptr);

    dets_t *dets=malloc(sizeof(dets));
	dets->width = (*infptr)->width;
	dets->height = (*infptr)->height;
	dets->color_type = (*infptr)->color_type;
	dets->bit_depth = (*infptr)->bit_depth;

	dets->npasses = png_set_interlace_handling(*pngptr);
	png_read_update_info(*pngptr, *infptr);

	/* read file */
	if (setjmp(png_jmpbuf(*pngptr)))
		abort_("[read_png_file] Error during read_image");

	*rptrs = malloc(sizeof(png_bytep) * dets->height);
	for (y=0; y<dets->height; y++)
		*rptrs[y] = (png_byte*) malloc((*infptr)->rowbytes);

	png_read_image(*pngptr, *rptrs);

    fclose(fp);
    return dets;
}

int main(int argc, char **argv)
{
	if (argc != 2)
		abort_("Usage: program_name <file_in>");

    png_structp *pngptr=NULL;
    png_infop *infptr=NULL;
    png_bytep *rptrs=NULL;

	dets_t *dets=read_png_file(argv[1], pngptr, infptr, &rptrs);

    printf("w:%d h:%d\n", dets->width, dets->height);
    free(dets);

    return 0;
}
