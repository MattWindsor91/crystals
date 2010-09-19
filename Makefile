WARN := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
	-Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
	-Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
	-Wconversion -Wstrict-prototypes
CFLAGS := -ggdb $(WARN)

.PHONY: all clean modules

%.so : %.c
	@gcc $(CFLAGS) -Wall -fPIC -rdynamic -c $^ -o $(^:.c=.o)
	@gcc -shared -Wl,-soname,$(^:.c=.so) -o $(^:.c=.so) $(^:.c=.o)

modules: modules/test.so

clean:
	-@rm loader &>/dev/null
	-@rm *.o &>/dev/null
	-@rm *.so &>/dev/null
	-@rm */*.o &>/dev/null
	-@rm */*.so &>/dev/null

loader: loader.c
	@gcc $(CFLAGS) -o loader loader.c -ldl >/dev/null

all: loader modules