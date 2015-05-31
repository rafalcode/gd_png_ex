#include <stdio.h>
#include <stdlib.h>
#include <gd.h>

#define OW 640 /* output image width */
#define OH OW /* output image width */
#define MSQ 25

int gcd(int a, int b)
{
    int temp;
    while (b != 0) {
        temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

int main(int argc, char *argv[])
{
    if(argc==1) {
        printf("Error. Pls supply png arguments.\n");
        exit(EXIT_FAILURE);
    }

    unsigned nims=atoi(argv[1]);
    unsigned i, sq;
    for(i=1;i<MSQ;++i) {
//         if(argc-1<(i*i)) {
        if(nims<(i*i)) {
            sq=i;
            break;
        }
    }
    printf("%d\n", sq); 
    unsigned ww=OW/sq;
    unsigned hh=OH/sq;
    unsigned wwa=calloc(ww, sizeof(unsigned));
    unsigned hha=calloc(hh, sizeof(unsigned));
    for(i=1;i<sq;++i) {
        wwa[i]=wwa[i-1]+ww;
        hha[i]=hha[i-1]+hh;
    }



       gdImagePtr imi, imo;
       int x, y;
       FILE *in, *out;

    for(i=0;i<sq;++i) {
    for(j=0;j<sq;++j) {
        in = fopen(argv[j*sq+i+1], "rb");
        gdImageCopyResized(imo, imi, wwa[i], hha[j], 0, 0, imi->sx, imi->sy);
    }
    // we'll use the first image to work out stuff
    in = fopen(argv[1], "rb");
    im_in = gdImageCreateFromPng(in);
    fclose(in);

    float wtimes=(float)OW/sq*imi->sx;
    float htimes=(float)OH/sq*imi->sy;
    float ttimes;
    ttimes=(wtimes<htimes)? wtimes : htimes;

    // now for the rest
    for(i=2;i<argc;++i) {
    in = fopen(argv[i], "rb");
    im_in = gdImageCreateFromPng(in);
    fclose(in);

// Make the output image four times as large on both axes
im_out = gdImageCreate(im_in->sx * 4, im_in->sy * 4);
// Now tile the larger image using the smaller one
for (y = 0; (y < 4); y++)
for (x = 0; (x < 4); x++)
gdImageCopy(im_out, im_in, x * im_in->sx, y * im_in->sy, 0, 0, im_in->sx, im_in->sy);

out = fopen(argv[2], "wb");
gdImagePng(im_out, out);
fclose(out);
gdImageDestroy(im_in);
gdImageDestroy(im_out);
*/
return 0;
}
