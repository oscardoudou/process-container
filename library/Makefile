CFLAGS := -m64 -O2 -g -D_GNU_SOURCE -D_REENTRANT -W -I/usr/local/include
LDFLAGS := -m64 -lm

all: pcontainer.c
	$(CC) $(CFLAGS) -Wall -fPIC -c pcontainer.c
	$(CC) $(CFLAGS) -shared -Wl,-soname,libpcontainer.so.1 -o libpcontainer.so.1.0 pcontainer.o

install: libpcontainer.so.1.0
	cp libpcontainer.so.1.0 /usr/lib/libpcontainer.so.1
	ln -fs /usr/lib/libpcontainer.so.1 /usr/lib/libpcontainer.so
	cp pcontainer.h  /usr/local/include


clean:
	rm -f *.so *.o *.1.0


.PHONY: all clean
