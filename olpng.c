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

typedef struct /* sol_t */
{
    int bgi, eni; /* beginning and end indices */
    int *ol; /* of size eni-bgi: values */
} sol_t;

void abort_(const char *s, ...)
{
    va_list args;// whoa there's a struct I've never heard of! STDARG!!!
    va_start(args, s);
    vfprintf(stderr, s, args);
    fprintf(stderr, "\n");
    va_end(args);
    abort();
}

png_bytep *read_png_file(char* file_name, int *w, int *h, png_byte *color_type, png_byte *bit_depth, png_infop *info_ptrin)
{
    int y;
    unsigned char header[8];	// 8 is the maximum size that can be checked: // I put unisgned here .. and get no warnings!

    /* open file and test for it being a png */
    FILE *fp = fopen(file_name, "rb");
    if (!fp)
        abort_("[read_png_file] File %s could not be opened for reading", file_name);
    fread(header, 1, 8, fp);
    if (png_sig_cmp(header, 0, 8)) //differs in signedness warning here! SOLN make header unsigned!
        abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);

    png_structp png_ptr;
    png_infop info_ptr=*info_ptrin;

    /* initialize stuff */
    if( !(png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
        abort_("[read_png_file] png_create_read_struct failed");

    if( !(info_ptr = png_create_info_struct(png_ptr)))
        abort_("[read_png_file] png_create_info_struct failed");

    if(setjmp(png_jmpbuf(png_ptr)))
        abort_("[read_png_file] Error during init_io");

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);

    png_read_info(png_ptr, info_ptr);

    *w = info_ptr->width;
    *h = info_ptr->height;
    *color_type = info_ptr->color_type;
    *bit_depth = info_ptr->bit_depth;

    /* I think there's only two values here, and one is extremely common and the other is very unusual
       int number_of_passes = png_set_interlace_handling(png_ptr);
       */
    png_read_update_info(png_ptr, info_ptr);

    /* read file */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[read_png_file] Error during read_image");

    png_bytep *row_ptrs;
    row_ptrs = (png_bytep*) malloc(sizeof(png_bytep) * (*h));
    for (y=0; y<(*h); y++)
        row_ptrs[y] = (png_byte*) malloc(info_ptr->rowbytes);

    png_read_image(png_ptr, row_ptrs);

    fclose(fp);
    *info_ptrin=info_ptr;
    // png_read_end(png_ptr, NULL); // getting IDAT CRC error here
    png_destroy_read_struct(&png_ptr, NULL, NULL);

    return row_ptrs;
}

void write_png_file(char* file_name, int w, int h, png_byte color_type, png_byte bit_depth, png_bytep *row_ptrs)
{
    /* create file */
    FILE *fp = fopen(file_name, "wb");
    if (!fp)
        abort_("[write_png_file] File %s could not be opened for writing", file_name);

    png_structp png_ptr;
    png_infop info_ptr;

    /* initialize stuff */
    if( !(png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
        abort_("[write_png_file] png_create_write_struct failed");

    if( !(info_ptr = png_create_info_struct(png_ptr)))
        abort_("[write_png_file] png_create_info_struct failed");

    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during init_io");

    png_init_io(png_ptr, fp);

    /* write header */
    if(setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during writing header");

    png_set_IHDR(png_ptr, info_ptr, w, h, bit_depth, color_type, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during writing bytes");

    png_write_image(png_ptr, row_ptrs);

    /* end write */
    if (setjmp(png_jmpbuf(png_ptr)))
        abort_("[write_png_file] Error during end of write");

    png_write_end(png_ptr, NULL); // fine here, but not in the read, where I tried 
    png_destroy_write_struct(&png_ptr, &info_ptr);

    fclose(fp);
}

void process_file(int w, int h, png_bytep *row_ptrs, png_infop info_ptr)
{
    if (info_ptr->color_type != PNG_COLOR_TYPE_RGB)
        abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGB (is %d)", info_ptr->color_type);

    int x, y;
    png_byte *row, *ptr;
    // sol_t lol[3], bol[3], rol[3], tol[3];
    sol_t lol[3];
    for (y=0; y<h; y++) {
        row = row_ptrs[y];
        for (x=0; x<w; x++) {
            ptr = &(row[x*3]);
            if (y==21)
                printf("Pixel at position [ %d - %d ] has the following RGB values: %d - %d - %d\n", x, y, ptr[0], ptr[1], ptr[2]);
#endif
            /* The processing now begins ... set red value to 0 and green value to the blue one */
            ptr[0] = 0;
            ptr[1] = ptr[2];
        }
    }
    return;
}

int main(int argc, char **argv)
{
    if (argc != 3)
        abort_("Usage: program_name <file_in> <file_out>");

    int w, h, y;
    png_byte color_type, bit_depth;
    png_infop info_ptr;

    png_bytep *row_ptrs=read_png_file(argv[1], &w, &h, &color_type, &bit_depth, &info_ptr);

    process_file(w, h, row_ptrs, info_ptr);

    write_png_file(argv[2], w, h, color_type, bit_depth, row_ptrs);

    for (y=0; y<h; y++)
        free(row_ptrs[y]);
    free(row_ptrs);
    png_destroy_write_struct(NULL, &info_ptr);

    return 0;
}
