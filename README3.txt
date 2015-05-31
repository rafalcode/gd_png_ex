OK an attenpt "to gather all the cats"
in terms of all my libgd code (codelets)

Start off with splim, which splits an input JPG and outputs (most)
of its subimages as PNGs. For usage with metapixel.
Works rather OK.
Used the splij code (which was the one I made for splitting the Nokia 4x4 images).

>>> HAve included some libpng sutff here as well ... have'nt quite used it much though.

libpng
------
to get info .. man libpng is OK
look at png.h and pngconf.h (which is probably included in by png.h

png_byte? it's simply an insigned char! But don't print it as such, because obviously its a byte sized container for their own use.  Cast to int instead.

libpng tends to use 0-255 integer values for tis colors (RGBA)

libpng seems to be fast but it's not what I want right now.

Havinglooked at libcair and libpng to domy bloakc within bloak experiment,
I finally settle on libgd, until then I see that it is not installed. Such is the stuff!

libpng: also uses double coords to locate the pixels.

YOu'd better check this blog entry
http://latentcontent.net/2007/12/05/libpng-worst-api-ever/

explains the error reporting convention
of libpng is to use setjmp, 
a sort of exception, goto type way which is a total pain.
but actually I think people just live with it.

It appears that the RGBA values are held as a pointer at each pixel, i.e.

        for (y=0; y<height; y++) {
                png_byte* row = row_pointers[y];
                for (x=0; x<width; x++) {
                        png_byte* ptr = &(row[x*4]);


As you can see you have the height and width but you have to dereference the value at a row element to give you
the array.

