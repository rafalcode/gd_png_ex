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
