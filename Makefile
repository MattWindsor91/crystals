BIN      := maprender-test

OBJ      := main.o graphics.o map.o mapview.o events.o module.o
SOBJ     := modules/gfx-sdl.so

SOURCES  := $(subst .o,.c,$(OBJ))
DEPFILES := $(subst .o,.d,$(OBJ))

CC       := clang
RM       := rm -f

MODPATH  := $(shell pwd)/modules/\0

WARN     := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes -ansi

LIBS     := `sdl-config --libs` -lSDL_image -g
CFLAGS   := `sdl-config --cflags` -ansi -pedantic -O2 -ggdb \
           -DMODPATH="\"$(MODPATH)\"" $(WARN)

.PHONY: all doc autodoc clean clean-doc

### Binaries

all: $(BIN) doc autodoc

$(BIN): $(OBJ) $(SOBJ)
	@echo -n "Linking... "

	@$(CC) $(OBJ) -o "$(BIN)" $(LIBS)
	@rm -rf *.d

	@echo "done."

-include $(DEPFILES)

%.d: %.c
	@echo -n "Generating dependency makefile for $<... "

	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) -f $@.$$$$

	@echo "done."

%.so : %.c
	@echo -n "Compiling $< to shared object... "

	@$(CC) $(CFLAGS) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o)
	@$(CC) $(LIBS) -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o)

	@echo "done."

%.o : %.c
	@echo -n "Compiling $<... "

	@$(CC) -c $< $(CFLAGS) -o $@

	@echo "done."

### Documentation

doc: doc/module.pdf doc/mapformat-internal.pdf

autodoc:
	@echo -n "Running doxygen... "

	@doxygen >/dev/null

	@echo "done."

%.pdf: %.tex
	@echo -n "LaTeXing $<... "

	@pdflatex -output-directory=$(shell dirname $@) $^ >/dev/null

	@echo "done."

### Cleaning

clean: clean-obj clean-doc

clean-obj:
	@echo -n "Cleaning (object files)... "

	-@$(RM) $(BIN) *.o *.d *.so modules/*.so modules/*.o &

	@echo "done."

clean-doc:
	@echo -n "Cleaning (documentation)... "

	-@$(RM) doc/*.pdf doc/*.aux doc/*.log
	-@$(RM) -r autodoc

	@echo "done."