WARN   = -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
         -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
         -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
         -Wconversion -Wstrict-prototypes
CFLAGS = -ggdb -O2 $(WARN)
CC     = clang

.PHONY: clean tests

%.so : %.c
	@$(CC) $(CFLAGS) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o)
	@$(CC) -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o)

clean:
	-@rm tests/module     &>/dev/null
	-@rm *.o *.so         &>/dev/null
	-@rm */*.o */*.so     &>/dev/null
	-@rm */*/*.o */*/*.so &>/dev/null

tests: module.c tests/module.c tests/modules/test.so
	@$(CC) $(CFLAGS) -ldl -o tests/module -DTESTSUITE -DMODPATH="\"$(shell pwd)/tests/modules/\"" module.c tests/module.c
	@./tests/module
