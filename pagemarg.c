/*
 * Copyright 2002-2008 Guillaume Cottenceau.
 *
 * This software may be freely redistributed under the terms
 * of the X11 license.
 *
 * RFnotes: supposedly this is an easy test of libpng ...
 * but it isn't really , note hte three dots in the first function's arguments: WTF?
 * is that from stdarg.h?
 *
 *  this was for finding when a page appears on a baisc blackground
 *  but I found black was registering as 0/0/255 what?
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

typedef struct /* pxpa_t, pixel path type */
{
    int *y, *x;
    int bf;
    int sz;
} pxpa_t;

typedef struct /* i_t, pair of indices */
{
    int hb, he; /* beginning, end index on horizontal */
    int vb, ve; /* beginning, end index on vertical */
    int *h1, *h2; /* array from either hor side */
    int *v1, *v2; /* array from either ver side */
} i_t;

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

    *w = png_get_image_width(png_ptr, info_ptr);
    *h = png_get_image_height(png_ptr, info_ptr);
    *color_type = png_get_color_type(png_ptr, info_ptr);
    *bit_depth = png_get_bit_depth(png_ptr, info_ptr);

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
        row_ptrs[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr, info_ptr));

    png_read_image(png_ptr, row_ptrs);

    fclose(fp);
    *info_ptrin=info_ptr;
    // png_read_end(png_ptr, NULL); // getting IDAT CRC error here

    /* OK now to avoid memory leaks ...
     * here was the original, that DID cause problems: 
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    then I read here:
    https://stackoverflow.com/questions/11149216/memory-leak-in-libpng-or-am-i-just-plain-dumb
    I should be including hte info_ptr as here: */
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL); // this then worked.

    return row_ptrs;
}

void diffinlin0(int w, int h, int bthresh, png_byte color_type, png_byte bit_depth, png_bytep *row_ptrs)
{
    /* difference in line */
    int i, j, x, y;

    png_byte *row, *ptr;
    int col[2]={0};
    int mark=0;
    int lev1=12, lev2=14;
    for (y=lev1; y<lev2; y++) {
        row = row_ptrs[y];
        for (x=0; x<w; x++) {
            ptr = row+x*3;
            if( (ptr[0] < bthresh) & (ptr[1] < bthresh) & (ptr[2] < bthresh)) {
                col[0]=x;
                mark=1;
            } else if( (ptr[0] > bthresh) & (ptr[1] > bthresh) & (ptr[2] > bthresh) & mark) {
                col[1]=x;
                mark=0;
            }
        }
    }
    printf("left mark = %i, right mark = %i\n", col[0], col[1]);
}

void diffinlin2(int w, int h, int bthresh, png_byte color_type, png_byte bit_depth, png_bytep *row_ptrs)
{
    /* difference in line */
    int i, j, x, y;

    png_byte *row, *ptr;
    png_byte ptr2[3]={255, 255, 255};
    int lev1=12, lev2=14;
    for (y=lev1; y<lev2; y++) {
        row = row_ptrs[y];
        for (x=0; x<w; x++) {
            ptr = row+x*3;
            // if( (ptr2[0] < bthresh) & (ptr2[1] < bthresh) & (ptr2[2] < bthresh)) {
            // if( (ptr2[0] < ptr[0]) & (ptr2[1] < ptr[1]) & (ptr2[2] < ptr[2]) ) {
            if( (ptr2[0] < ptr[0]) & (ptr2[1] < ptr[1]) & (ptr2[2] < ptr[2]) & (ptr2[0] < bthresh) & (ptr2[1] < bthresh) & (ptr2[2] < bthresh)) {
                printf("%i\n", x);
            }
            ptr2[0] = ptr[0];
            ptr2[1] = ptr[1];
            ptr2[2] = ptr[2];
            printf("%i/%i/%i\n", ptr[0], ptr[1], ptr[2]); 
        }
    }
}

void diffinlin3(int w, int h, int bthresh, png_byte color_type, png_byte bit_depth, png_bytep *row_ptrs, char *fn)
{
    /* difference in line */
    int i, j, x, y;

    png_byte *row, *ptr;
    png_byte ptr2[3]={255, 255, 255}; // initial value, will then hold last value in loop.
    y=h/2; //the vertical midpoint of the image!
    row = row_ptrs[y];
    int nw; //new width
    int lpyes=0; // left point found.
    int leftx=0;

    // search from the left.
    for (x=0; x<w; x++) {
        ptr = row+x*3;
        if( (ptr2[0] < ptr[0]) & (ptr2[1] < ptr[1]) & (ptr2[2] < ptr[2]) & ((ptr[0] - ptr2[0])> 20)) {
            nw=w-x;
            lpyes=1;
            leftx=x;
        }
        if(lpyes)
            break;
        ptr2[0] = ptr[0];
        ptr2[1] = ptr[1];
        ptr2[2] = ptr[2];
    }
    int rpyes=0;
    ptr2[0]=ptr2[1]=ptr2[2]=255;
    // search from the right.
    for (x=w-1; x>=0; x--) {
        ptr = row+x*3;
        // printf("ptr0:%i\n", ptr[0]); 
        if( (ptr2[0] < ptr[0]) & (ptr2[1] < ptr[1]) & (ptr2[2] < ptr[2]) & ((ptr[0] - ptr2[0])> 20)) {
            nw=nw-(w-x);
            // printf("change on right found at x=%i. Your imagemagick command:\n", x);
            // printf("convert %s -crop %ix%i+%i+%i +repage <outfilename>\n", fn, nw, h, leftx, 0);
            rpyes=1;
        }
        if(rpyes)
            break;
        ptr2[0] = ptr[0];
        ptr2[1] = ptr[1];
        ptr2[2] = ptr[2];
        // printf("%i/%i/%i\n", ptr[0], ptr[1], ptr[2]); 
    }
    // search from top.
    ptr2[0]=ptr2[1]=ptr2[2]=255;
    x=w/2;
    int nh=h; // new height
    int tpyes=0;
    int top_y=0;
    for (y=0; y<h; y++) {
        row = row_ptrs[y];
        ptr = row+x*3;
        //  printf("ptr0:%i\n", ptr[0]); 
        if( (ptr2[0] < ptr[0]) & (ptr2[1] < ptr[1]) & (ptr2[2] < ptr[2]) & ((ptr[0] - ptr2[0])> 20)) {
            nh=h-y;
            top_y=y;
            tpyes=1;
        }
        if(tpyes)
            break;
        ptr2[0] = ptr[0];
        ptr2[1] = ptr[1];
        ptr2[2] = ptr[2];
    }
    // from bottom
    int bpyes=0;
    ptr2[0]=ptr2[1]=ptr2[2]=255;
    for (y=h-1; y>=0; y--) {
        row = row_ptrs[y];
        ptr = row+x*3;
        if( (ptr2[0] < ptr[0]) & (ptr2[1] < ptr[1]) & (ptr2[2] < ptr[2]) & ((ptr[0] - ptr2[0])> 20)) {
            nh=nh-(h-y);
            bpyes=1;
        }
        if(bpyes)
            break;
        ptr2[0] = ptr[0];
        ptr2[1] = ptr[1];
        ptr2[2] = ptr[2];
    }

    char cmdstr[256]={0};
    char newfn[64]={0};
    char *tmp=strrchr(fn, '.');
    sprintf(newfn, "%.*s.jpg", (int)(tmp-fn), fn);
    if(lpyes | rpyes | tpyes | bpyes) {
        sprintf(cmdstr, "convert %s -crop %ix%i+%i+%i +repage %s\n", fn, nw, nh, leftx, top_y, newfn);
        printf("Running following system command:\n%s\n", cmdstr); 
        system(cmdstr);
    }
}

int main(int argc, char **argv)
{
    if(argc!=2) {
        printf("Usage notes for \"%s\":\n", argv[0]);
        // printf("This program finds the black margins in an image. Args: 1) png image 2) \"-f\" to force image magick crop and conv to jpglack. 2,5, 10 etc.\n");
        printf("This program finds the black margins in an image. One args: png image.\n");
        exit(EXIT_FAILURE);
    }

    int bthresh=10; // used to arg 2 ... don't know what it is really ... the integer that constitutes black I suppose
    int w, h, y;
    png_byte color_type, bit_depth;
    png_infop info_ptr;

    png_bytep *row_ptrs=read_png_file(argv[1], &w, &h, &color_type, &bit_depth, &info_ptr);
    printf("submitted png, w=%i h=%i\n", w, h); 

    // diffinlin0(w, h, bthresh, color_type, bit_depth, row_ptrs);
    diffinlin3(w, h, bthresh, color_type, bit_depth, row_ptrs, argv[1]);

    for (y=0; y<h; y++)
        free(row_ptrs[y]);
    free(row_ptrs);
    png_destroy_write_struct(NULL, &info_ptr);

    return 0;
}
