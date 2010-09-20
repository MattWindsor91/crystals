WARN := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
	-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
	-Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
	-Wconversion -Wstrict-prototypes
CFLAGS := -ggdb $(WARN)

.PHONY: clean tests

%.so : %.c
	@gcc $(CFLAGS) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o)
	@gcc -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o)

clean:
	-@rm *.o &>/dev/null
	-@rm *.so &>/dev/null
	-@rm */*.o &>/dev/null
	-@rm */*.so &>/dev/null

tests: module.c tests/module.c tests/modules/test.so
	@gcc $(CFLAGS) -ldl -o tests/module -DTESTSUITE -DMODPATH="\"$(shell pwd)/tests/modules/\"" module.c tests/module.c >/dev/null
	@./tests/module
