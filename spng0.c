#include <stdio.h>
#include <malloc.h>
#include <png.h>

#define WID 16
#define HEI 16
#define FNAM "spng0.png"

int writeImage(char* filename, int width, int height, char* title)
{
	int ret = 0;
	FILE *fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;
	
	// Open file for writing (binary mode)
	fp = fopen(filename, "wb");
	if (fp == NULL) {
		fprintf(stderr, "Could not open file %s for writing\n", filename);
		ret = 2;
		goto finalise;
	}

	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		ret = 3;
		goto finalise;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		ret = 4;
		goto finalise;
	}

	// Setup Exception handling
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "Error during png creation\n");
		ret = 5;
		goto finalise;
	}

	png_init_io(png_ptr, fp);

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Set title
	if (title != NULL) {
		png_text title_text;
		title_text.compression = PNG_TEXT_COMPRESSION_NONE;
		title_text.key = "Title";
		title_text.text = title;
		png_set_text(png_ptr, info_ptr, &title_text, 1);
	}

	png_write_info(png_ptr, info_ptr);

	// Allocate memory for one row (3 bytes per pixel - RGB)
	row = (png_bytep) malloc(3 * width * sizeof(png_byte));

	// Write image data
	int x, y;
	for (y=0 ; y<height ; y++) {
		for (x=0 ; x<3*width ; x+=3) {
            // if(((y==3) & (x<4)) | ((x==7) & ((y>7) & (y<12)))) {
            if((x>3*3-1) & (x<3*4) & (y>=3) & (y<9)) {
                row[x]=0;
                row[x+1]=196;
                row[x+2]=128;
            } else if((x<3*3) & (y==3)) {
                row[x]=0;
                row[x+1]=196;
                row[x+2]=128;
            } else if((x>=3*3) & (x<3*16) & (y==9)) {
                row[x]=0;
                row[x+1]=196;
                row[x+2]=128;
            } else {
                row[x]=0;
                row[x+1]=0;
                row[x+2]=0;
            }
        }
        png_write_row(png_ptr, row);
    }

    // End write
    png_write_end(png_ptr, NULL);

finalise:
    if (fp != NULL)
        fclose(fp);
    if (info_ptr != NULL)
        png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    if (png_ptr != NULL)
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
    if (row != NULL)
        free(row);

    return ret;
}

int main(int argc, char *argv[])
{
    // Specify an output image size
    int width = WID;
    int height = HEI;

    writeImage(FNAM, width, height, "My spng0 image");

    return 0;
}
