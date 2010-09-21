BIN      := maprender-test

OBJ      := main.o graphics.o map.o mapview.o events.o module.o
SOBJ     := modules/gfx-sdl.so modules/event-sdl.so

SOURCES  := $(subst .o,.c,$(OBJ))
DEPFILES := $(subst .o,.d,$(OBJ))

CC       := clang
RM       := rm -f

MODPATH  := $(shell pwd)/modules/\0

WARN     := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes -ansi

LIBS     := -ldl -g
CFLAGS   := -ansi -pedantic -O2 -ggdb -DMODPATH="\"$(MODPATH)\"" $(WARN)

SOLIBS   := $(LIBS) `sdl-config --libs` -lSDL_image
SOCFLAGS := $(CFLAGS) `sdl-config --cflags`

.PHONY: all doc autodoc clean clean-doc

### Binaries

all: $(BIN) doc autodoc

$(BIN): $(OBJ) $(SOBJ)
	@echo "Linking ${BIN}... "

	@$(CC) $(OBJ) -o "$(BIN)" $(LIBS)
	@rm -rf *.d

-include $(DEPFILES)

%.d: %.c
	@echo "Generating dependency makefile for $<... "

	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) -f $@.$$$$

%.so : %.c
	@echo "Compiling $< to shared object... "

	@$(CC) $(SOCFLAGS) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o)
	@$(CC) $(SOLIBS) -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o)

%.o : %.c
	@echo "Compiling $<... "

	@$(CC) -c $< $(CFLAGS) -o $@

### Documentation

doc: doc/module.pdf doc/mapformat-internal.pdf

autodoc:
	@echo "Running doxygen... "

	@doxygen >/dev/null

%.pdf: %.tex
	@echo "LaTeXing $<... "

	@pdflatex -output-directory=$(shell dirname $@) $^ >/dev/null

### Cleaning

clean: clean-obj clean-doc

clean-obj:
	@echo "Cleaning (object files)... "

	-@$(RM) $(BIN) *.o *.d *.so modules/*.so modules/*.o &

clean-doc:
	@echo "Cleaning (documentation)... "

	-@$(RM) doc/*.pdf doc/*.aux doc/*.log
	-@$(RM) -r autodoc
