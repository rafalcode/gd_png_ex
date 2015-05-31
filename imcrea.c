#include <stdio.h>
#include <gd.h>

#define GW 640 /* grid width */
#define GH GW /* grid height */
#define BW 120 /* grid width */
#define BH 100 /* grid height */
#define BPx 40 /* starting pt of block in xdir */
#define BPy 200 /* starting pt of block in ydir */

int main(int argc, char *argv[])
{

	gdImagePtr im;
	
	int c1, c2;
	
	FILE *out;
	
	/* Create the image */
	
	im = gdImageCreate(GW, GH);
	
	/* Allocate background */
	c2 = gdImageColorAllocate(im, 210, 180, 140);
	
	/* Allocate drawing color */
	
	c1 = gdImageColorAllocate(im, 95, 158, 160); // CadetBlue
	
	/* Draw rectangle */
	gdImageRectangle(im, 50, 50, 99, 99, c1);
	gdImageRectangle(im, 51, 51, 98, 98, c1);
	gdImageRectangle(im, 52, 52, 97, 97, c1);
	
	/* Open output file in binary mode */
	out = fopen("rect.png", "wb");
	
	/* Write PNG */
	gdImagePng(im, out);
	
	/* Close file */
	
	fclose(out);
	
	/* Destroy image */
	
	gdImageDestroy(im);
	return 0;
}
