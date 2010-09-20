BIN      := maprender-test

SOURCES  := main.c graphics.c map.c mapview.c events.c module.c
OBJ      := $(subst .c,.o,$(SOURCES))
DEPFILES := $(subst .c,.d,$(SOURCES))
SO       := modules/gfx-sdl.so

CC       := clang

MODPATH  := $(shell pwd)/modules/\0

WARN     := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes -ansi

LIBS     := `sdl-config --libs` -lSDL_image -g
CFLAGS   := `sdl-config --cflags` -ansi -pedantic -O2 -ggdb -DDEBUG \
            -DDEFMODPATH="\"$(MODPATH)\"" $(WARN)

.PHONY: all doc clean clean-tests clean-doc tests

all: $(BIN)

doc: doc/module.pdf

$(BIN): $(OBJ) $(SO)
	@$(CC) $(OBJ) -o "$(BIN)" $(LIBS) >/dev/null

-include $(DEPFILES)

%.o : %.c
	@$(CC) -c $< $(CFLAGS) -o $@ >/dev/null

clean: clean-tests clean-doc
	-@rm $(BIN) *.o *.d *.so modules/*.so modules/*.o &>/dev/null

clean-tests:
	-@rm tests/*.{o,so} tests/modules/*.{o,so} &>/dev/null

clean-doc:
	-@rm doc/*.{pdf,aux,log} &>/dev/null

tests: module.c tests/module.c tests/modules/test.so
	@echo "Compiling test suite:"
	@$(CC) $(CFLAGS) -ldl -o tests/module -DTESTSUITE -DMODPATH="\"$(shell pwd)/tests/modules/\"" module.c tests/module.c
	@echo
	@echo "Running test suite:"
	@./tests/module 2>/dev/null
	@echo
	@echo "Running memory leak check:"
	@valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./tests/module 2>&1 | grep lost

%.d: %.c
	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.so : %.c
	@$(CC) $(CFLAGS) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o) >/dev/null
	@$(CC) $(LIBS) -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o) >/dev/null

%.pdf: %.tex
	@pdflatex -output-directory=$(shell dirname $@) $^ >/dev/null