#include <stdio.h>
#include <stdlib.h>
#include <gd.h>
#include <math.h>

int main(int argc, char *argv[])
{
    if(argc==1) {
        printf("Error. Pls supply square number png arguments.\n");
        exit(EXIT_FAILURE);
    }
    if(((argc-1)!= 4) & ((argc-1)!= 9) & ((argc-1)!= 16) & ((argc-1)!= 25)) {
        printf("Error. Pls supply square number png arguments.\n");
        exit(EXIT_FAILURE);
    }
    gdImagePtr im_in=NULL, im_out;
    int x, y;
    FILE *in, *out;
    unsigned xy=(unsigned)sqrt((float)argc-1);
    printf("xy=%d\n",xy); 
    /* Make the output image four times as large on both axes */
    in = fopen(argv[1], "rb");
    im_in = gdImageCreateFromPng(in);
    im_out = gdImageCreateTrueColor(im_in->sx * xy, im_in->sy * xy);
    //im_out = gdImageCreate(im_in->sx * xy, im_in->sy * xy);
            fclose(in);
            gdImageCopy(im_out, im_in, 0, 0, 0, 0, im_in->sx, im_in->sy);

    for (y=0; y<xy; y++)
        for (x=0; x<xy; x++) {
            if((y*xy+x+1)==1)
                continue;
    in = fopen(argv[y*xy+x+1], "rb");
    im_in = gdImageCreateFromPng(in);

            fclose(in);
            gdImageCopy(im_out, im_in, x * im_in->sx, y * im_in->sy, 0, 0, im_in->sx, im_in->sy);
        }

    out = fopen("imt.png", "wb");
    gdImagePng(im_out, out);
    fclose(out);
    gdImageDestroy(im_in);
    gdImageDestroy(im_out);
    return 0;
}
