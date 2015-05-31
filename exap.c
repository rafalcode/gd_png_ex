/*
 * Examine a png file
 *
 * Copyright 2002-2011 Guillaume Cottenceau and contributors.
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

/* global vars */
int x, y;

int width, height, rowbytes;
png_byte color_type;
png_byte bit_depth;
png_byte nchans;
png_byte ilace;

png_structp png_ptr;
png_infop info_ptr;
int number_of_passes;
png_bytep *rwptrs;

void abort_(const char *s, ...)
{
    va_list args;
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}

void read_png_file(char* file_name)
{
    unsigned char header[8];    // 8 is the maximum size that can be checked

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp)
        abort_("[read_png_file] File %s could not be opened for reading", file_name);
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8))
        abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);

    /* initialize stuff */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
        abort_("[read_png_file] png_create_read_struct failed");

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
        abort_("[read_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[read_png_file] Error during init_io");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8); /* refers to png signature not the color depth! */

    png_read_info(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    color_type = png_get_color_type(png_ptr, info_ptr);
    bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    nchans= png_get_channels(png_ptr, info_ptr);
    printf("Inf: W=%d, H=%d, CT=%d, BD=%d, NC=%d\n", width, height, color_type, bit_depth, nchans); 

    /* work out interlace */
    ilace= png_get_interlace_type(png_ptr, info_ptr);
    if(ilace==1)
        printf("Image is not interlaced.\n"); 
    else {
        printf("Image is interlaced.\n"); 
        number_of_passes = png_set_interlace_handling(png_ptr); /* returns an int: 7 if interlaced, 1 if not */
        png_read_update_info(png_ptr, info_ptr);
    }

    /* read file */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[read_png_file] Error during read_image");

    rwptrs = (png_bytep*) malloc(sizeof(png_bytep) * height);

    if (bit_depth == 16)
        rowbytes = width*8;
    else
        rowbytes = width*4;

    for (y=0; y<height; y++)
        rwptrs[y] = (png_byte*) malloc(rowbytes);

    png_read_image(png_ptr, rwptrs);

    fclose(fp);

    for (y=0; y<height; y++)
        free(rwptrs[y]);
    free(rwptrs);
}

void process_file(void)
{
    for (y=0; y<height; y++) {
        png_byte* row = rwptrs[y];
        for (x=0; x<width; x++) {
            png_byte* ptr = &(row[x*4]);
            if(y==100)
                printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n", x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

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

    read_png_file(argv[1]);
    process_file();

    return 0;
}
