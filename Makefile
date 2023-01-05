DIST_FILENAME=SDLApp
DIST_TYPE=lib
include ../Common/Base.mk
include ../Tensor/Include.mk

# this is in common with Include.mk:

DYNAMIC_LIBS_osx+=$(HOME)/lib/libSDL2-2.0.0$(LIB_SUFFIX)
LIBS_linux+=SDL2
INCLUDE_osx+=$(HOME)/include/SDL2
INCLUDE_linux+=/usr/include/SDL2
#DYNAMIC_LIBS_linux+=/usr/lib/x86_64-linux-gnu/libSDL2-2.0$(LIB_SUFFIX)
#LIBS+=SDL2main
