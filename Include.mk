#any dependent projects need these:
SDLAPP_PATH:=$(dir $(lastword $(MAKEFILE_LIST)))

INCLUDE+=$(SDLAPP_PATH)/include

DYNAMIC_LIBS+=$(SDLAPP_PATH)dist/$(PLATFORM)/$(BUILD)/libSDLApp$(LIB_SUFFIX)

# this is in common with Makefile:

DYNAMIC_LIBS_osx+=$(HOME)/lib/libSDL2-2.0.0$(LIB_SUFFIX)
LIBS_linux+=SDL2
INCLUDE_osx+=$(HOME)/include/SDL2
INCLUDE_linux+=/usr/include/SDL2
#DYNAMIC_LIBS_linux+=/usr/lib/x86_64-linux-gnu/libSDL2-2.0$(LIB_SUFFIX)
#LIBS+=SDL2main
