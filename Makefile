BIN       = maprender-test

SOURCES  = main.c graphics.c map.c mapview.c events.c module.c
OBJ      = $(subst .c,.o,$(SOURCES))
DEPFILES = $(subst .c,.d,$(SOURCES))
SO       = modules/gfx-sdl.so

CC        = clang

MODPATH   = $(shell pwd)/modules/\0

WARN   = -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
         -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
         -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
         -Wconversion -Wstrict-prototypes -ansi

LIBS      = `sdl-config --libs` -lSDL_image -g
CFLAGS    = `sdl-config --cflags` -ansi -pedantic -O2 -ggdb -DDEBUG \
            -DMODPATH="\"$(MODPATH)\"" $(WARN)

.PHONY: clean

all: $(BIN)

doc: doc/module.pdf

$(BIN): $(OBJ) $(SO)
	$(CC) $(OBJ) -o "$(BIN)" $(LIBS)
	rm -rf *.d

-include $(DEPFILES)

%.o : %.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -f $(BIN) *.o *.d *.so 
	rm -f modules/*.so modules/*.o 
	rm -f doc/*.pdf doc/*.aux doc/*.log

%.d: %.c
	@echo "Generating dependency makefile for $<."
	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.so : %.c
	$(CC) $(CFLAGS) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o)
	$(CC) $(LIBS) -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o)

%.pdf: %.tex
	pdflatex -output-directory=$(shell dirname $@) $^ >/dev/null
