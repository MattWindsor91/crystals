BIN      = maprender-test

OBJ      = main.o graphics.o map.o mapview.o events.o module.o
SOBJ     = modules/gfx-sdl.so

SOURCES  = $(subst .o,.c,$(OBJ))
DEPFILES = $(subst .o,.d,$(OBJ))

CC       = clang
RM       = rm -f

MODPATH  = $(shell pwd)/modules/\0

WARN     = -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
           -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
           -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
           -Wconversion -Wstrict-prototypes -ansi

LIBS     = `sdl-config --libs` -lSDL_image -g
CFLAGS   = `sdl-config --cflags` -ansi -pedantic -O2 -ggdb \
           -DMODPATH="\"$(MODPATH)\"" $(WARN)

.PHONY: clean autodoc

all: $(BIN) doc autodoc

doc: doc/module.pdf doc/mapformat-internal.pdf

$(BIN): $(OBJ) $(SOBJ)
	$(CC) $(OBJ) -o "$(BIN)" $(LIBS)
	rm -rf *.d

-include $(DEPFILES)

%.d: %.c
	@echo "Generating dependency makefile for $<."
	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) -f $@.$$$$

%.so : %.c
	@$(CC) $(CFLAGS) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o)
	@$(CC) $(LIBS) -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o)

%.o : %.c
	@$(CC) -c $< $(CFLAGS) -o $@

%.pdf: %.tex
	@pdflatex -output-directory=$(shell dirname $@) $^ >/dev/null

clean:
	-@$(RM) $(BIN) *.o *.d *.so
	-@$(RM) modules/*.so modules/*.o
	-@$(RM) doc/*.pdf doc/*.aux doc/*.log
	-@$(RM) -r autodoc

autodoc:
	@doxygen

