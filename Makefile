# Crystals (working title)
#
# Copyright (c) 2010, 2011 Matt Windsor, Michael Walker and Alexander
#                          Preisinger.
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#   * Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#   * Redistributions in binary form must reproduce the above
#     copyright notice, this list of conditions and the following
#     disclaimer in the documentation and/or other materials provided
#     with the distribution.
#
#   * The names of contributors may not be used to endorse or promote
#     products derived from this software without specific prior
#     written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# AFOREMENTIONED COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
# STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
# OF THE POSSIBILITY OF SUCH DAMAGE.

### MAKEFILE ###

# The following Makefile is set up by default to allow for compilation
# to a few common platform targets.
#
# For the most part, all users need do is change the PLATFORM
# definition to refer to the platform they wish to compile for.
# However, fine-tuning of other variables may be necessary.

# For convenience, anything that you, the user, should consider
# modifying to suit your needs is marked with chevrons (eg >> PLATFORM <<).

# In contrast, anything marked ! is an action performed by the Makefile
# that in most cases should NOT be tampered with.


## >> PLATFORM << ##

# Allowed platforms:

# windows-mingw32 | Windows 32- and 64-bit (9x onwards) using MinGW
#                 | (set up for cross-compiler i486-mingw32 by default)
# gnu             | GNU distributions
# gnu-linux       | GNU/Linux distributions (eg Ubuntu) - implies gnu

# TODO: Add BSD, native Windows.

PLATFORM := gnu-linux


## >> EXECUTABLE NAME << ##

# Note: On certain platforms, this will have an extension appended
#       to it.

BIN      := crystals


## >> DIRECTORIES << ##

# Prefix appended to tool names.  This generally only needs to change
# for cross-compilation on *nix systems.

BUILDPREFIX := /usr

# Source directory, relative to this directory.
# This probably shouldn't be changed.

SRCDIR   := src

# Subdirectory of this directory and of SRCDIR in which modules and
# module sources respectively are stored.
# This probably shouldn't be changed.

MODDIR   := modules

# Subdirectory of this directory in which documentation is stored.
# This probably shouldn't be changed.

DOCDIR   := doc

# Subdirectory of SRCDIR in which bindings code is stored.
# This probably shouldn't be changed.

BNDDIR   := bindings

# Subdirectory of this directory and of SRCDIR in which unit tests
# are stored.
# This probably shouldn't be changed.

TESTDIR  := tests

# Subdirectory of SRCDIR in which platform-specific code (eg Windows hacks)
# is stored.
# This probably shouldn't be changed.

PLATDIR  := platform

## TEST DIRECTORIES ##

TESTS    := module optionparser parser

# ! Add TESTDIR to all test paths #

TESTS    := $(addprefix $(TESTDIR)/,$(TESTS))


## CORE OBJECTS ##

# The following object list represents the core of the engine
# and thus generally does not need altering by users.

OBJ      := main.o graphics.o events.o file.o
OBJ      += util.o module.o optionparser.o parser.o state.o
OBJ      += field/field.o field/objectset.o
OBJ      += field/object.o field/object-api.o field/object-image.o
OBJ      += map/map.o map/mapview.o map/maprender.o map/mapload.o


# Note: DO NOT add .so or .dll onto the end of module names!
# This will be done later, after platform-specific configuration.

# Graphics backends #


# You will need at least one of these in order to run crystals.
# The graphics backends interface with graphics libraries in order to
# render crystals on screen.

# gfx-sdl | Simple DirectMedia Layer graphics backend.
#         | This renders in software, but is cross-platform and
#         | likely to be the most stable backend.  (Recommended)
#         |
#         | Works on:  windows, gnu and gnu variants.
#         | NOTE: Use event-sdl if using this backend.

SOBJ     := gfx-sdl gfx-dummy

# Events backends #

# You will need at least one of these in order to run crystals.
# The events backends interface with events libraries (often part of
# graphics libraries) in order to accept input from the user via
# keyboard, mouse etc.

# event-sdl | Simple DirectMedia Layer events backend.
#           | This is REQUIRED if using a SDL-based graphics driver.
#           | (Recommended)
#           |
#           | Works on:  windows, gnu and gnu variants.

SOBJ     += event-sdl event-dummy

# Scripting bindings #

# These allow for the use of scripting languages with the crystals
# engine.

# lua    | Lua bindings backend.
# python | Python bindings backend.
# dummy  | Dummy bindings for memory tests.

BINDINGS := dummy

## Documentation ##

DOC      := module.pdf mapformat-internal.pdf optionparser.pdf

# ! Add DOCDIR to all documentation paths #

DOC      := $(addprefix $(DOCDIR)/,$(DOC))


## >> TOOLCHAIN AND COMPILER FLAGS << ##

# Note: the following may be overridden by platform-specific
# options below.  For example, by default the windows platform
# overrides gcc with a cross-compiler.

# The compiler to use.

CC       := gcc

# The delete command.

RM       := rm -f

# The method of getting the distribution name.

DIST	 := $(shell uname -r | sed "s/.*-//")

# The module path.

MODPATH  := $(shell pwd)/$(MODDIR)/\0

# Warning flags to use during compilation.
# These should be kept on!

WARN     := -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wstrict-prototypes


# Not possible to use with -ansi cflag
#ifeq ($(BINDINGS), ruby)
#	LIBS     += -lruby
#	CFLAGS   += -I/usr/include/ruby-1.9.1 \
#		    -I/usr/include/ruby-1.9.1/i686-linux/
#endif

# Preprocessor defines from configure.

DEFS := -DPACKAGE_NAME=\"CRYSTALS\" -DPACKAGE_TARNAME=\"crystals\" -DPACKAGE_VERSION=\"0.0\" -DPACKAGE_STRING=\"CRYSTALS\ 0.0\" -DPACKAGE_BUGREPORT=\"\" -DPACKAGE_URL=\"\" -DHAVE_LIBSDL=1 -DHAVE_LIBSDL_IMAGE=1 -DHAVE_LIBGLIB_2_0=1 -DHAVE_LIBGMODULE_2_0=1 -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE__BOOL=1 -DHAVE_STDBOOL_H=1 -DHAVE_LIMITS_H=1 -DHAVE_STDINT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_STDLIB_H=1 -DHAVE_MALLOC=1 -DHAVE_MEMSET=1 -DHAVE_STRSTR=1

# Generic compilation flags.

CFLAGS   := -g -O2   -ansi -pedantic -O2 -g3 -ggdb \
	    -DDEFMODPATH="\"$(MODPATH)\"" ${DEFS} $(WARN)

## >> PLATFORM SPECIFICS << ##

# Flags used:

# NO_STRSAFE | On Windows platforms, do not use strsafe.  At time of writing,
#            | this causes snprintf etc. to be used instead, which depends on
#            | C99 or GNU support.
# USE_STDINT | On Windows platforms, use stdint.h (depends on C99 or
#            | GNU support) instead of Windows API types.

GNUFLAGS := -DPLATFORM_GNU
GNULIBS  := -ldl -lpthread

# Windows using mingw32 #

ifeq ($(PLATFORM),windows-mingw32)
    # Most of this is setting up to use a cross-compiler,
    # so if using mingw32 natively much of this can be changed
    # or commented out.

	CC          := i486-mingw32-gcc
	BUILDPREFIX := /usr/i486-mingw32
	PKGCONFIG   := i486-mingw32-pkg-config
	CFLAGS      += -DPLATFORM_WINDOWS -DNO_STRSAFE -DUSE_STDINT
	LIBS        += -L/usr/i486-mingw32/lib -lmingw32 -mwindows
	DLLEXT      := dll
	BINSUFFIX   := .exe

	# Add Windows-specific code

	OBJ         += $(PLATDIR)/w32-main.o $(PLATDIR)/w32-util.o
        OBJ         += $(PLATDIR)/w32-module.o

endif


# GNU #

ifeq ($(PLATFORM),gnu)
	CFLAGS    += $(GNUFLAGS)
	LIBS      += $(GNULIBS)
	PKGCONFIG := $(BUILDPREFIX)/bin/pkg-config
	DLLEXT    := so
endif


# GNU/Linux #

ifeq ($(PLATFORM),gnu-linux)
	CFLAGS    += $(GNUFLAGS) -DPLATFORM_GNU_LINUX
	LIBS      += $(GNULIBS)
	PKGCONFIG := $(BUILDPREFIX)/bin/pkg-config
	DLLEXT    := so
endif


# FIXME
CFLAGS   += `pkg-config glib-2.0 gmodule-2.0 --cflags`
LIBS     += `pkg-config glib-2.0 gmodule-2.0 --libs`


# Add bindings object file to the other object files and add the proper CFLAGS and LIBS.

OBJ      += $(BNDDIR)/$(BINDINGS).o

ifeq ($(BINDINGS), python)
	LIBS     += `python-config --libs`
	CFLAGS   += `python-config --includes` -DNDEBUG
endif

ifeq ($(BINDINGS), lua)
	LIBS     += `$(PKGCONFIG) --libs lua`
endif

## ! LAST-MINUTE AUTOMATED MAGIC ##

# ! Add SRCDIR to all object paths #

OBJ      := $(addprefix $(SRCDIR)/,$(OBJ))

# ! Add SRCDIR and MODDIR to all shared object paths #

SOBJ     := $(addprefix $(MODDIR)/,$(SOBJ))
SOBJ     := $(addprefix $(SRCDIR)/,$(SOBJ))

# ! Add DLLEXT too #

SOBJ     := $(addsuffix .$(DLLEXT),$(SOBJ))

# ! Add suffix to program name #

BIN      := $(addsuffix $(BINSUFFIX),$(BIN))

## ! Make lists for sources and dependency files ##

SOURCES  := $(subst .o,.c,$(OBJ))
DEPFILES := $(subst .o,.d,$(OBJ))

## ! RULES ##

.PHONY: all doc autodoc clean clean-tests clean-doc clean-modules modules tests copy

all: $(BIN) copy
alldoc: all doc autodoc

$(BIN): $(OBJ) modules
	@echo "Linking..."
	@$(CC) $(OBJ) -o "$(BIN)" $(LIBS) >/dev/null

copy:
	@echo "Copying modules to destination..."
	-@mkdir -p modules
	@cp -r $(SRCDIR)/$(MODDIR)/* $(MODDIR)

	@echo "Copying tests to destination..."
	-@mkdir -p tests
	@cp -r $(SRCDIR)/$(TESTDIR)/* $(TESTDIR)


clean: clean-tests clean-doc clean-modules clean-bindings
	@echo "Cleaning..."
	-@$(RM) $(BIN) $(SRCDIR)/*.{o,d,$(DLLEXT)} &>/dev/null
	-@$(RM) $(BIN) $(SRCDIR)/field/*.{o,d,$(DLLEXT)} &>/dev/null
	-@$(RM) $(BIN) $(SRCDIR)/$(PLATDIR)/*.{o,d,$(DLLEXT)} &>/dev/null


# Modules #

$(SRCDIR)/$(MODDIR)/gfx-sdl.$(DLLEXT): LIBS   += `$(BUILDPREFIX)/bin/sdl-config --libs` -lSDL_image
$(SRCDIR)/$(MODDIR)/gfx-sdl.$(DLLEXT): CFLAGS += `$(BUILDPREFIX)/bin/sdl-config --cflags`

$(SRCDIR)/$(MODDIR)/event-sdl.$(DLLEXT): LIBS   += `$(BUILDPREFIX)/bin/sdl-config --libs`
$(SRCDIR)/$(MODDIR)/event-sdl.$(DLLEXT): CFLAGS += `$(BUILDPREFIX)/bin/sdl-config --cflags`

modules: $(SOBJ)

clean-modules:
	@echo "Cleaning modules..."
	-@$(RM) $(MODDIR)/*.{$(DLLEXT),o} &>/dev/null
	-@$(RM) $(SRCDIR)/$(MODDIR)/*.{$(DLLEXT),o} &>/dev/null


# Documentation #

doc: $(DOC)

autodoc:
	@echo "Running doxygen..."
	@doxygen >/dev/null

clean-doc:
	@echo "Cleaning documentation..."
	-@$(RM) doc/*.{pdf,aux,log} &>/dev/null
	-@$(RM) -r autodoc

clean-bindings:
	@echo "Cleaning bindings..."
	-@$(RM) $(SRCDIR)/$(BNDDIR)/*.{o,d}

### Test Suite

tests: CFLAGS += -DTESTSUITE -DMODPATH="\"$(shell pwd)/$(TESTDIR)/$(MODDIR)/\""
#tests: $(TESTS)
#	@echo "Running tests..."
#	@for file in $(TESTS); do $$file &>/dev/null || echo "Test '$$file' failed."; done

clean-tests:
	@echo "Cleaning tests..."
	-@$(RM) $(TESTS) $(TESTDIR)/*.{o,so} $(TESTDIR)/$(MODDIR)/*.{o,so} &>/dev/null

# File Types #

%.o : %.c
	@echo "Compiling $<..."
	@$(CC) -c $< $(CFLAGS) -o $@ >/dev/null

%.d: %.c
	@echo "Generating dependency makefile for $<..."
	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) -f $@.$$$$

%.so : %.c
	@echo "Compiling $< to shared object..."
	@$(CC) $(CFLAGS) $(INCLUDES) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o) >/dev/null
	@$(CC) -shared -Wl,-soname,$(^:.c=.$(DLLEXT)) -o $(^:.c=.$(DLLEXT)) $(^:.c=.o) $(LIBS) >/dev/null

%.dll : %.c
	@echo "Compiling $< to dynamic linked library..."
	@$(CC) $(CFLAGS) $(INCLUDES) -Wall -c $^ -o $(^:.c=.o) >/dev/null
	@$(CC) -shared -Wl -o $(^:.c=.dll) $(^:.c=.o) $(LIBS) >/dev/null

%.pdf: %.tex
	@echo "LaTeXing $<..."
	@pdflatex -output-directory=$(shell dirname $@) $^ >/dev/null


# Include dependency makefiles here. #

-include $(DEPFILES)

# vim: set ts=8 noexpandtab:
