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
CFLAGS    = `sdl-config --cflags` -ansi -pedantic -O2 -g -DDEBUG \
            -DMODPATH="\"$(MODPATH)\"" $(WARN)

.PHONY: clean

all: $(BIN)

$(BIN): $(OBJ) $(SO)
	$(CC) $(OBJ) -o "$(BIN)" $(LIBS)
	rm -rf *.d

-include $(DEPFILES)

%.o : %.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -f *.o *.d

%.d: %.c
	@echo "Generating dependency makefile for $<."
	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.so : %.c
	$(CC) $(CFLAGS) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o)
	$(CC) $(LIBS) -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o)

