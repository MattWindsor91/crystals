BIN      := maprender-test

TESTS    := tests/module tests/optionparser
OBJ      := main.o graphics.o map.o mapview.o events.o module.o optionparser.o
SOBJ     := modules/gfx-sdl.so modules/event-sdl.so

SOURCES  := $(subst .o,.c,$(OBJ))
DEPFILES := $(subst .o,.d,$(OBJ))

CC       := clang
RM       := rm -f

MODPATH  := $(shell pwd)/modules/\0

WARN     := -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes

LIBS     := -ldl -g
CFLAGS   := -ansi -pedantic -O2 -ggdb -DDEFMODPATH="\"$(MODPATH)\"" $(WARN)

.PHONY: all doc autodoc clean clean-tests clean-doc clean-modules modules tests

all: $(BIN) doc autodoc modules

$(BIN): $(OBJ) $(SO)
	@echo "Linking..."
	@$(CC) $(OBJ) -o "$(BIN)" $(LIBS) >/dev/null

-include $(DEPFILES)

clean: clean-tests clean-doc clean-modules
	@echo "Cleaning..."
	-@$(RM) $(BIN) *.o *.d *.so &>/dev/null

### Modules
modules/gfx-sdl.so: LIBS   += `sdl-config --libs` -lSDL_image
modules/gfx-sdl.so: CFLAGS += `sdl-config --cflags`

modules/event-sdl.so: LIBS   += `sdl-config --libs`
modules/event-sdl.so: CFLAGS += `sdl-config --cflags`

modules: $(SOBJ)

clean-modules:
	@echo "Cleaning modules..."
	-@$(RM) modules/*.{so,o} &>/dev/null

### Documentation
doc: doc/module.pdf doc/mapformat-internal.pdf

autodoc:
	@echo "Running doxygen..."
	@doxygen >/dev/null

clean-doc:
	@echo "Cleaning documentation..."
	-@$(RM) doc/*.{pdf,aux,log} &>/dev/null
	-@$(RM) -r autodoc

### Test Suite
tests: CFLAGS += -DTESTSUITE -DMODPATH="\"$(shell pwd)/tests/modules/\""
tests: $(TESTS)
	@echo "Running tests..."
	@for file in $(TESTS); do $$file &>/dev/null || echo "Test '$$file' failed."; done

tests/module: module.o tests/module.o tests/modules/test.so
	@$(CC) $(CFLAGS) module.o tests/module.o tests/modules/test.so -o $@ >/dev/null

tests/optionparser: optionparser.o tests/optionparser.o
	@$(CC) $(CFLAGS) optionparser.o tests/optionparser.o -o $@ >/dev/null

clean-tests:
	@echo "Cleaning tests..."
	-@$(RM) $(TESTS) tests/*.{o,so} tests/modules/*.{o,so} &>/dev/null

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
	@$(CC) $(CFLAGS) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o) >/dev/null
	@$(CC) $(LIBS) -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o) >/dev/null

%.pdf: %.tex
	@echo "LaTeXing $<..."
	@pdflatex -output-directory=$(shell dirname $@) $^ >/dev/null
