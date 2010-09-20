BIN       = maprender-test

SOURCES  = main.c graphics.c map.c mapview.c events.c
OBJ      = $(subst .c,.o,$(SOURCES))
DEPFILES = $(subst .c,.d,$(SOURCES))

CC        = clang

DISMALROOT = ../../

LIBS      = `sdl-config --libs` -lSDL_image -g
CFLAGS    = `sdl-config --cflags` -ansi -pedantic -O2 -g -DDEBUG \
            -I$(DISMALROOT) -Wall -Wextra

.PHONY: clean

all: $(BIN)

$(BIN): $(OBJ)
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
