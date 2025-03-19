# gd\_png\_ex

Sorry for the terrible name. I wanted to rename .. but it's a bit too late
seeing as some og ht code here is from 2010 and earlier.

What's it about? programming examples of libgd and libpng in action

(Quite clearly you need to have those libraries and their header files installed in your system in order to compiler)0.

# Programs
## Early proof-of-api-use programs
* exap: compile with 
* chapng: change png: takes an RGB and sets does a simple manipulation of the colours.

# Working and hopefully useful programs
* magpng: magnifies a full png (usually very small ones) without any manipulation, so that we see a highly pixelated version. 
Actually this is a kronecker product operation. TODO: magnify not the enitre input png, but rather a region of one.
(Jst to note that by version 51f460c, when it started working properly, I was also getting a valgrind clean bill of health,
which means that sort of thing is possible in libpng).

# undocumented bits of code
simpng.c this was created from magpng ... it's almost the same. Not sure what I had in mind.

# rect234.png
You'll look at this and say. Did I generate this? Well the random x positioing of the rectangles is probab;y difficult
and the name of te file suggests ... an inkscape template!

# pagemarg.c
takes pang with black background and fins the let, right, top bottom points where it ends that central page in the png begins.
