# gd\_png\_ex
programming examples of libgd and libpng in action
CLearly you need to have those libraries and their header files installed in hyour system
in order to compile.

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
