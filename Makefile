DIST_FILENAME=SDLApp
DIST_TYPE=lib
include ../Common/Base.mk
include ../Tensor/Include.mk

# this is in common with Include.mk:

DYNAMIC_LIBS_osx+=$(HOME)/lib/libSDL3-3.0.0$(LIB_SUFFIX)
LIBS_linux+=SDL3
INCLUDE_osx+=$(HOME)/include/SDL3
INCLUDE_linux+=/usr/include/SDL3
#DYNAMIC_LIBS_linux+=/usr/lib/x86_64-linux-gnu/libSDL3-3.0$(LIB_SUFFIX)
#LIBS+=SDL3main
