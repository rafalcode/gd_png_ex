/*
 * Copyright 2002-2008 Guillaume Cottenceau.
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

    int x, y, i, j;
    png_byte *row, *ptr;
    i_t *lsci=calloc(3, sizeof(i_t)); /*Mne: Line Stop Channel Indices: for each RGB channel, indices of stop (pixel value changes from 4 dirs */
    int bav[3];
    for(i=0;i<3;++i) 
        bav[i]=(int)row_ptrs[2][2*3+i]; /* background pixel value, at 2,2 for each of the three, second pixel starts at 6 */
    unsigned char *nocha=malloc(3*sizeof(unsigned char));
    memset(nocha, 1, 3*sizeof(unsigned char));

#ifdef DBG2
    printf("bavs: ");
    for(i=0;i<3;++i) 
        printf("%d ", bav[i]);
    printf("\n");
#endif

    /* first, we start on left and look for first y index that holds a chnage from background val */
    for (y=0; y<h; y++) {
        row = row_ptrs[y];
        for (x=0; x<w; x++) {
            ptr = &(row[x*3]);
            for(j=0;j<3;++j) 
                if((ptr[j] != bav[j]) & nocha[j]) {
                    lsci[j].hb=y;
                    nocha[j]=0;
                }
            if( !nocha[0] & !nocha[1] & !nocha[2])
                goto out1;
        }
    }
out1: for(i=0;i<3;++i) 
          printf("lbi=%d; ", lsci[i].hb);
      printf("\n"); 

      for(i=0;i<3;++i) 
          nocha[i]=1;
      for (y=h-1; y>=0; --y) {
          row = row_ptrs[y];
          for (x=0; x<w; x++) {
              ptr = &(row[x*3]);
              for(j=0;j<3;++j) 
                  if((ptr[j] != bav[j]) & nocha[j]) {
                      lsci[j].he=y;
                      nocha[j]=0;
                  }
              if( !nocha[0] & !nocha[1] & !nocha[2])
                  goto out2;
          }
      }
out2: for(i=0;i<3;++i) 
          printf("lei=%d; ", lsci[i].he);
      printf("\n"); 

      /* at this point we have to choose between the 3 starting and 3 ending indices*/
      /* choose max of former and min of latter */

      int fir=lsci[0].hb /* first row with stopval */, lar=lsci[0].he /* last row with stopval*/;
      for(i=1;i<3;++i) {
          if(lsci[i].hb>fir)
              fir=lsci[i].hb;
          if(lsci[i].he<lar)
              lar=lsci[i].he;
      }
      printf("fir:%d, lar:%d\n", fir, lar); 
      for(i=0;i<3;++i) 
          lsci[i].h1=malloc((lar-fir+1)*sizeof(int));

      /* first from the first side (i.e. left */
      /* but we also stealth get our vb and ve values */
      for(j=0;j<3;++j) {
          lsci[j].vb=0x7FFFFFFF;
          lsci[j].ve=0;

      }
      for (y=fir; y<=lar; y++) {
          row = row_ptrs[y];
          memset(nocha, 1, 3*sizeof(unsigned char));
          for (x=0; x<w; x++) {
              ptr = &(row[x*3]);
              for(j=0;j<3;++j) 
                  if((ptr[j] != bav[j]) & nocha[j]) {
                      lsci[j].h1[y-fir]=x;
                      if(lsci[j].vb>x)
                          lsci[j].vb=x; /* looking for the smallest value of x from the left */
                      nocha[j]=0;
                  }
              if( !nocha[0] & !nocha[1] & !nocha[2])
                  break; /* i.e. go to next row */
          }
      }

      /* second from the second horizontal side (i.e. right */
      for(i=0;i<3;++i) 
          lsci[i].h2=malloc((lar-fir+1)*sizeof(int));
      for (y=fir; y<=lar; y++) {
          row = row_ptrs[y];
          memset(nocha, 1, 3*sizeof(unsigned char));
          for (x=w-1; x>=0; --x) {
              ptr = &(row[x*3]);
              for(j=0;j<3;++j) 
                  if((ptr[j] != bav[j]) & nocha[j]) {
                      lsci[j].h2[y-fir]=x;
                      if(lsci[j].ve<x)
                          lsci[j].ve=x; /* looking for the highest value of x from the left */
                      nocha[j]=0;
                  }
              if( !nocha[0] & !nocha[1] & !nocha[2])
                  break; /* i.e. go to next row */
          }
      }
#ifdef DBG
      printf("Listing rownum:rl/gl/bl-rr/gr/br (row stop points from left-right:\n"); 
      for(i=0;i<lar-fir+1;++i) {
          printf("%d:", i+fir);
          for(j=0;j<3;++j) 
              printf((j==2)?"%d":"%d/", lsci[j].h1[i]);
          printf("-"); 
          for(j=0;j<3;++j) 
              printf((j==2)?"%d":"%d/", lsci[j].h2[i]);
          printf(" "); 
      }
      printf("\n"); 
#endif

      /* having done from left and right, you may have noticed that we got ve and vb in the process */
      int fic=lsci[0].vb /* first column with stopval */, lac=lsci[0].ve /* last column with stopval*/;
      for(i=1;i<3;++i) {
          if(lsci[i].vb>fic)
              fic=lsci[i].vb;
          if(lsci[i].ve<lac)
              lac=lsci[i].ve;
      }
      printf("fic:%d, lac:%d\n", fic, lac); 
      /* notice AGAIN, that we've chosen the most aggressive of the colours */

      /* capture stopvals from top */
      for(i=0;i<3;++i) 
          lsci[i].v1=malloc((lac-fic+1)*sizeof(int));

      for (x=fic; x<=lac; x++) {
          memset(nocha, 1, 3*sizeof(unsigned char));
          for (y=0; y<h; y++) {
              for(j=0;j<3;++j) 
                  if((row_ptrs[y][x*3+j] != bav[j]) & nocha[j]) {
                      lsci[j].v1[x-fic]=y;
                      nocha[j]=0;
                  }
              if( !nocha[0] & !nocha[1] & !nocha[2])
                  break; /* i.e. go to next column */
          }
      }
      /* capture stopvals from bottom */
      for(i=0;i<3;++i) 
          lsci[i].v2=malloc((lac-fic+1)*sizeof(int));

      for (x=fic; x<=lac; x++) {
          memset(nocha, 1, 3*sizeof(unsigned char));
          for (y=h-1; y>=0; --y) {
              for(j=0;j<3;++j) 
                  if((row_ptrs[y][x*3+j] != bav[j]) & nocha[j]) {
                      lsci[j].v2[x-fic]=y;
                      nocha[j]=0;
                  }
              if( !nocha[0] & !nocha[1] & !nocha[2])
                  break; /* i.e. go to next column */
          }
      }
#ifdef DBG
      printf("Listing colnum:rt/gt/bt-rb/gb/bb (row stop points from top-bottom:\n"); 
      for(i=0;i<lac-fic+1;++i) {
          printf("%d:", i+fic);
          for(j=0;j<3;++j) 
              printf((j==2)?"%d":"%d/", lsci[j].v1[i]);
          printf("-"); 
          for(j=0;j<3;++j) 
              printf((j==2)?"%d":"%d/", lsci[j].v2[i]);
          printf(" "); 
      }
      printf("\n"); 
#endif
      /* SETTING PIXELS STAGE */
      /* OK, we have our information, now  we can set the background to anything we want */
      /* first off, right to first stop val, left to last stopval */
      /* temp fir and temp lar, useful for vert ops */
      // int vfic, vlac; /* Very first ir, very last row */
      // int cfic, clac; /* Current fir and lar, temporary, but their last values will be re-used */
      int *fica=malloc((lar-fir+1)*sizeof(int));
      int *laca=malloc((lar-fir+1)*sizeof(int));
      for (y=0; y<h; y++) {
          if( (y<fir) | (y>lar) ) {
              for (x=0; x<w; x++)
                  for(j=0;j<3;++j) 
                      row_ptrs[y][3*x+j] = 128;
          } else {
              fica[y-fir]=lsci[0].h1[y-fir];
              laca[y-fir]=lsci[0].h2[y-fir];
              for(i=1;i<3;++i) {
                  if(lsci[i].h1[y-fir]>fica[y-fir])
                      fica[y-fir]=lsci[i].h1[y-fir];
                  if(lsci[i].h2[y-fir]<laca[y-fir])
                      laca[y-fir]=lsci[i].h2[y-fir];
              }
              for (x=0; x<fica[y-fir]; x++)
                  for(j=0;j<3;++j) 
                      row_ptrs[y][3*x+j] = 128;
              for (x=w-1; x>laca[y-fir]; --x)
                  for(j=0;j<3;++j) 
                      row_ptrs[y][3*x+j] = 128;
          }
      }

      /* I finished up one night here, and the next day I found picking up the thread pretty
       * difficult. Anyhow, for the from-top and from-bottom traces, there are only two horiz intervals
       * we need worry about for top to first stop val, bottom to last stopval operations:
       * vfir - vlar and cfir - clar */
      int tfir, tlar;
      for(j=0;j<(lar-fir+1); j++) {
          for (x=fica[j]; x<=laca[j]; x++) {
              tfir=lsci[0].v1[x-fic];
              tlar=lsci[0].v2[x-fic];
              for(i=1;i<3;++i) {
                  if(lsci[i].v1[x-fic]>tfir)
                      tfir=lsci[i].v1[x-fic];
                  if(lsci[i].v2[x-fic]<tlar)
                      tlar=lsci[i].v2[x-fic];
              }
              for (y=fir; y<=tfir; y++)
                  for(j=0;j<3;++j) 
                      row_ptrs[y][3*x+j] = 128;
              for (y=lar; y>=tlar; --y)
                  for(j=0;j<3;++j) 
                      row_ptrs[y][3*x+j] = 128;
          }
      }

      /* free-up stuff */
      for(i=0;i<3;++i) {
          free(lsci[i].h1);
          free(lsci[i].h2);
          free(lsci[i].v1);
          free(lsci[i].v2);
      }
      free(fica);
      free(laca);
      free(lsci);
      free(nocha);
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
