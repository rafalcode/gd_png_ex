/*
 * Examine a png file
 *
 * Copyright 2002-2011 Guillaume Cottenceau and contributors.
 * Mods Ramón Fallon.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

typedef struct /* infcntr: info container */
{
    int w, h, rbytes;
    int numpasses;
    png_byte ct; /* color_type */
    png_byte bd; /*bit_depth */
    png_byte nc;/* numchans */
    png_byte il; /* interlace? */
    png_bytep *rpta; /* an array of row pointers */
} infcntr;


void abort_(const char *s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}

void read_png_file(char* file_name, infcntr *infc)
{
    int j;
    png_structp png_sptr;
    png_infop info_ptr;
    unsigned char header[8];    // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp)
        abort_("[read_png_file] File %s could not be opened for reading", file_name);
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
        abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);

    /* initialize stuff */
    png_sptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_sptr)
        abort_("[read_png_file] png_create_read_struct failed");

    info_ptr = png_create_info_struct(png_sptr);
    if (!info_ptr)
        abort_("[read_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_sptr)))
        abort_("[read_png_file] Error during init_io");

    png_init_io(png_sptr, fp);
    png_set_sig_bytes(png_sptr, 8); /* refers to png signature not the color depth! */

    png_read_info(png_sptr, info_ptr);

    infc->w = png_get_image_width(png_sptr, info_ptr);
    infc->h = png_get_image_height(png_sptr, info_ptr);
    infc->ct = png_get_color_type(png_sptr, info_ptr);
    infc->bd = png_get_bit_depth(png_sptr, info_ptr);
    infc->nc= png_get_channels(png_sptr, info_ptr);

    /* work out interlace */
    infc->il= png_get_interlace_type(png_sptr, info_ptr);
    // if(infc->il==1)
    /* PNG_INTERLACE_NONE is 0 and PNG_INTERLACE_ADAM7 is 1 */
    if(infc->il== PNG_INTERLACE_NONE)
        printf("Image is not interlaced.\n"); 
    else if(infc->il == PNG_INTERLACE_ADAM7) {
        printf("Image is interlaced.\n"); 
        infc->numpasses = png_set_interlace_handling(png_sptr); /* returns an int: 7 if interlaced, 1 if not */
        png_read_update_info(png_sptr, info_ptr);
    } else
        printf("Warning: Odd return from interlace inquiry.\n"); 

    /* read file */
    if (setjmp(png_jmpbuf(png_sptr)))
        abort_("[read_png_file] Error during read_image");

    infc->rpta = malloc(sizeof(png_bytep) * infc->h);

    if (infc->bd == 16)
        infc->rbytes = infc->w*infc->nc*2;
    else
        infc->rbytes = infc->w*infc->nc;

    for (j=0; j<infc->h; j++)
        infc->rpta[j] = malloc(infc->rbytes);

    png_read_image(png_sptr, infc->rpta);

    fclose(fp);
}

void process_file(infcntr *infc)
{
    int i, j, k;
    png_byte *crptr=NULL /* current row pointer */, *pdptr=NULL /* pixel data pointer */;
    for (j=0; j<infc->h; j++) {
        crptr = infc->rpta[j];
        for (i=0; i<infc->w; i++) {
            pdptr = crptr+i*infc->nc;
            if(j==10) {
                printf("Pixel at position [ %d - %d ] has RGBA values: ", i, j);
                for(k=0;k<infc->nc;++k) 
                    printf("%d ", pdptr[k]);
                printf("\n"); 
            }

            /* perform whatever modifications needed, for example to set red value to 0 and green value to the blue one:
               ptr[0] = 0;
               ptr[1] = ptr[2]; */
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
        abort_("Usage: program_name <file_in>");

    int j;
    infcntr *infc=malloc(sizeof(infcntr));

    read_png_file(argv[1], infc);
    printf("Inf: W=%d, H=%d, CT=%d, BD=%d, NC=%d\n", infc->w, infc->h, infc->ct, infc->bd, infc->nc); 

    // process_file(infc);

    for (j=0; j<infc->h; j++)
        free(infc->rpta[j]);
    free(infc->rpta);
    free(infc);

    return 0;
}
