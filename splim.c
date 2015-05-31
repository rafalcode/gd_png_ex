/* Function to read a Jpeg image file name from 
 * and split it into 16 subimages (based on Nikon c16 mutlshot schema */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gd.h>
/* these two for making the directory into which output files will be placed*/
#include <sys/stat.h>                                                                                                                                 
#include <sys/types.h>                                                                                                                                
/* this is the crucial parameter: subimage X and Y extents */
#define SBIMEXTX 16
#define SBIMEXTY SBIMEXTX
/* for output directory and filenames */
#define DN_BFSZ 16 /*  dirname buffer size */
#define FN_BFSZ 32 /* filename buffer size (will include dirname too, actually */

int main(int argc, char *argv[])
{
    /*---- declarations ----*/
    gdImagePtr imi /* image in */, imo /* image out ... will be more than just one of these */;
    FILE *in, *out;
    int i, j, k, count, numsbimx, numsbimy, lostpxright, lostpxbottom;
    char *num, *fnam, *dnam; /*  the various components of the output dir & filename */
    /*---- argument checking ----*/
    if(argc != 3) {
        printf("Error. Pls supply 1st) input image filename, and 2nd) name for directory to be created, into which subimage files will go.\n");
        exit(EXIT_FAILURE);
    }
    in = fopen(argv[1], "rb");
    /*---- memory allocations ----*/
    num=malloc(sizeof(char)*DN_BFSZ);
    dnam=malloc(sizeof(char)*DN_BFSZ);
    fnam=malloc(sizeof(char)*FN_BFSZ);
    /*  pull in the image to be cut up . assume it's a jpeg */
    imi = gdImageCreateFromJpeg(in);
    fclose(in);
    /* allocate an output image of the szie we want, this will be written over for each output subimage */
    imo=gdImageCreateTrueColor(SBIMEXTX, SBIMEXTY);
    /* Cut-warning ... small parts of the image (on right and at bottom) will be disregarded */
    lostpxright= imi->sx%SBIMEXTX;
    lostpxbottom = imi->sy%SBIMEXTY;
    if(lostpxright)
        printf("%i pixels on the rightmost part of image are disregarded.\n", lostpxright);
    if(lostpxbottom)
        printf("%i pixels at the bottom of image are disregarded.\n", lostpxbottom);

    printf("Image Size= %i x %i\n", imi->sx, imi->sy);
    numsbimx = imi->sx/SBIMEXTX;
    numsbimy = imi->sy/SBIMEXTY;
    printf("Num subimages= %i x %i\n", numsbimx, numsbimy);
    /*  let's get the output directory ready */
    sprintf(dnam, "%s", argv[2]); 
    mkdir(dnam, 0755);
    /*  OK, main loop coming up */
    count=0;
    k=0;
    for(j=0; j<numsbimx; ++j) {
        for(i=0; i<numsbimy; ++i) {
            k=j*numsbimx+i;
            /* arrange format of filename */
            strcpy(fnam,dnam); /* flush fnam with orginal dnam */
            sprintf(num, "/%05i.png", k); /* separately arrange number part of filename */
            strcat(fnam,num);
            /* destfile, scrfile, desttopleftx, desttoplefty, srctopleftx, srctoplefty, copycanvasextenx, copycanvasexteny */
            out = fopen(fnam, "wb");
            gdImageCopy(imo, imi, 0, 0, j*SBIMEXTX, i*SBIMEXTY, SBIMEXTX, SBIMEXTY);
            gdImagePng(imo, out); /* Yes, that's right we're rendering Jpeg into Png */
            fclose(out);
            count++;
        }
    }
    printf("%i subimages created.\n", count); 

    free(num);
    free(dnam);
    free(fnam);
    gdImageDestroy(imi);
    gdImageDestroy(imo);
    return 0;
}
