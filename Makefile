BIN      := crystals

## Directories ##

SRCDIR   := src
MODDIR   := modules
DOCDIR   := doc
TESTDIR  := tests

## Test directories ##

TESTS    := module optionparser parser

# Add TESTDIR to all test paths #

TESTS    := $(addprefix $(TESTDIR)/,$(TESTS))

## Objects to link into main executable ##

OBJ      := main.o hash.o graphics.o events.o
OBJ      += util.o bindings.o module.o optionparser.o parser.o state.o
OBJ      += field/field.o field/map.o field/mapview.o field/object.o
OBJ      += field/object-api.o

# Add SRCDIR to all object paths #

OBJ      := $(addprefix $(SRCDIR)/,$(OBJ))

## Shared objects (modules) ##

SOBJ     := gfx-sdl.so event-sdl.so bindings-python.so bindings-lua.so

# Add SRCDIR and MODDIR to all shared object paths #

SOBJ     := $(addprefix $(MODDIR)/,$(SOBJ))
SOBJ     := $(addprefix $(SRCDIR)/,$(SOBJ))

## Sources and dependency files ##

SOURCES  := $(subst .o,.c,$(OBJ))
DEPFILES := $(subst .o,.d,$(OBJ))

## Documentation ##

DOC      := module.pdf mapformat-internal.pdf optionparser.pdf

# Add DOCDIR to all documentation paths #

DOC      := $(addprefix $(DOCDIR)/,$(DOC))

## Compilation toolchain ##

CC       := gcc -ggdb
RM       := rm -f
DIST	 := $(shell uname -r | sed "s/.*-//")

MODPATH  := $(shell pwd)/$(SRCDIR)/$(MODDIR)/\0

WARN     := -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes

LIBS     := -ldl -lpthread 
CFLAGS   := -ansi -pedantic -O2 -DDEFMODPATH="\"$(MODPATH)\"" $(WARN)

## Rules ##

.PHONY: all doc autodoc clean clean-tests clean-doc clean-modules modules tests

all: $(BIN) doc autodoc modules

$(BIN): $(OBJ) $(SO)
	@echo "Linking..."
	@$(CC) $(OBJ) -o "$(BIN)" $(LIBS) >/dev/null

	@echo "Copying modules to destination..."
	-@mkdir -p modules
	@cp -r $(SRCDIR)/$(MODDIR)/* $(MODDIR)

	@echo "Copying tests to destination..."
	-@mkdir -p tests
	@cp -r $(SRCDIR)/$(TESTDIR)/* $(TESTDIR)

-include $(DEPFILES)

clean: clean-tests clean-doc clean-modules
	@echo "Cleaning..."
	-@$(RM) $(BIN) $(SRCDIR)/*.{o,d,so} &>/dev/null

### Modules
$(SRCDIR)/$(MODDIR)/gfx-sdl.so: LIBS   += `sdl-config --libs` -lSDL_image
$(SRCDIR)/$(MODDIR)/gfx-sdl.so: CFLAGS += `sdl-config --cflags`

$(SRCDIR)/$(MODDIR)/event-sdl.so: LIBS   += `sdl-config --libs` 
$(SRCDIR)/$(MODDIR)/event-sdl.so: CFLAGS += `sdl-config --cflags`

$(SRCDIR)/$(MODDIR)/bindings-python.so: LIBS   += `python-config --libs`
$(SRCDIR)/$(MODDIR)/bindings-python.so: CFLAGS += `python-config --cflags`

$(SRCDIR)/$(MODDIR)/bindings-lua.so: LIBS += `pkg-config --libs lua`

modules: $(SOBJ)

clean-modules:
	@echo "Cleaning modules..."
	-@$(RM) $(MODDIR)/*.{so,o} &>/dev/null
	-@$(RM) $(SRCDIR)/$(MODDIR)/*.{so,o} &>/dev/null

### Documentation
doc: $(DOC)

autodoc:
	@echo "Running doxygen..."
	@doxygen >/dev/null

clean-doc:
	@echo "Cleaning documentation..."
	-@$(RM) doc/*.{pdf,aux,log} &>/dev/null
	-@$(RM) -r autodoc

### Test Suite
tests: CFLAGS += -DTESTSUITE -DMODPATH="\"$(shell pwd)/$(TESTDIR)/$(MODDIR)/\""
tests: $(TESTS)
	@echo "Running tests..."
	@for file in $(TESTS); do $$file &>/dev/null || echo "Test '$$file' failed."; done

$(TESTDIR)/module: $(SRCDIR)/module.o $(SRCDIR)/$(TESTDIR)/module.o $(SRCDIR)/$(TESTDIR)/$(MODDIR)/test.so
	@$(CC) $(CFLAGS) $(SRCDIR)/module.o $(SRCDIR)/$(TESTDIR)/module.o -ldl -o $@ >/dev/null

$(TESTDIR)/optionparser: $(SRCDIR)/optionparser.o $(SRCDIR)/$(TESTDIR)/optionparser.o
	@$(CC) $(CFLAGS) $(SRCDIR)/optionparser.o $(SRCDIR)/$(TESTDIR)/optionparser.o -o $@ >/dev/null

$(TESTDIR)/parser: $(SRCDIR)/parser.o $(SRCDIR)/$(TESTDIR)/parser.o
	@$(CC) $(CFLAGS) $(SRCDIR)/parser.o $(SRCDIR)/$(TESTDIR)/parser.o -o $@ >/dev/null

clean-tests:
	@echo "Cleaning tests..."
	-@$(RM) $(TESTS) $(TESTDIR)/*.{o,so} $(TESTDIR)/$(MODDIR)/*.{o,so} &>/dev/null

### File Types
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
	@$(CC) $(LIBS) -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o) >/dev/null

%.pdf: %.tex
	@echo "LaTeXing $<..."
	@pdflatex -output-directory=$(shell dirname $@) $^ >/dev/null

# vim: noexpandtab:
