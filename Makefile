BIN      := maprender-test

TESTS    := module
SOURCES  := main.c graphics.c map.c mapview.c events.c module.c
OBJ      := $(subst .c,.o,$(SOURCES))
DEPFILES := $(subst .c,.d,$(SOURCES))
SO       := modules/gfx-sdl.so

CC       := clang

MODPATH  := $(shell pwd)/modules/\0

WARN     := -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes

LIBS     := `sdl-config --libs` -lSDL_image -g
CFLAGS   := `sdl-config --cflags` -ansi -pedantic -O2 -ggdb \
            -DDEFMODPATH="\"$(MODPATH)\"" $(WARN)

.PHONY: all doc clean clean-tests clean-doc tests

all: $(BIN)

doc: doc/module.pdf doc/mapformat-internal.pdf

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

tests: test-module
	@for file in $(TESTS); do ./tests/$$file &>/dev/null || echo "Test '$$file' failed."; done

test-module: module.c tests/module.c tests/modules/test.so
	@$(CC) $(CFLAGS) -ldl -o tests/module -DTESTSUITE -DMODPATH="\"$(shell pwd)/tests/modules/\"" module.c tests/module.c

%.d: %.c
	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.so : %.c
	@$(CC) $(CFLAGS) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o) >/dev/null
	@$(CC) $(LIBS) -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o) >/dev/null

%.pdf: %.tex
	@pdflatex -output-directory=$(shell dirname $@) $^ >/dev/null
