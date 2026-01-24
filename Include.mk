SDLAPP_PATH:=$(dir $(lastword $(MAKEFILE_LIST)))
INCLUDE+=$(SDLAPP_PATH)/include
DEPEND_LIBS+=$(SDLAPP_PATH)dist/$(PLATFORM)/$(BUILD)/$(LIB_PREFIX)SDLApp$(LIB_SUFFIX)

# this is in common with Makefile:

DYNAMIC_LIBS_osx+=$(HOME)/lib/libSDL3-3.0.0$(LIB_SUFFIX)
LIBS_linux+=SDL3
INCLUDE_osx+=$(HOME)/include/SDL3
INCLUDE_linux+=/usr/include/SDL3
#DYNAMIC_LIBS_linux+=/usr/lib/x86_64-linux-gnu/libSDL3-3.0$(LIB_SUFFIX)
#LIBS+=SDL3main
