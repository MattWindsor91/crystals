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
            -DDEFMODPATH="\"$(MODPATH)\"" $(WARN)

.PHONY: clean tests

all: $(BIN)

doc: doc/module.pdf

$(BIN): $(OBJ) $(SO)
	$(CC) $(OBJ) -o "$(BIN)" $(LIBS)
	rm -rf *.d

-include $(DEPFILES)

%.o : %.c
	$(CC) -c $< $(CFLAGS) -o $@

doc: doc/module.pdf

clean: clean-tests
	rm -f $(BIN) *.o *.d *.so modules/*.so modules/*.o doc/*.{pdf, aux, log}

clean-tests:
	rm -f tests/*.{o,so} tests/modules/*.{o,so}

tests: module.c tests/module.c tests/modules/test.so
	@echo "Compiling test suite:"
	@$(CC) $(CFLAGS) -ldl -o tests/module -DTESTSUITE -DMODPATH="\"$(shell pwd)/tests/modules/\"" module.c tests/module.c
	@echo
	@echo "Running test suite:"
	@./tests/module
	@echo
	@echo "Running memory leak check:"
	@valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./tests/module 2>&1 | grep lost

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
