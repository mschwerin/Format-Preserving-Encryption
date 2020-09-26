CFLAGS = -O2 -Wall -Wsign-compare -Wmissing-declarations -Werror -fPIC -std=c99
SO_LINKS = -lm -lcrypto

LIB = libfpe.a libfpe.so
EXAMPLE_SRC = example.c
EXAMPLE_EXE = example
OBJS = src/ff1.o src/ff3.o src/fpe_locl.o

all: $(LIB) $(EXAMPLE_EXE)

libfpe.a: $(OBJS)
	ar rcs $@ $(OBJS)

libfpe.so: $(OBJS)
	$(CC) -shared -fPIC -Wl,-soname,libfpe.so $(OBJS) $(SO_LINKS) -o $@

.PHONY = all clean

src/ff1.o: src/ff1.c
	$(CC) $(CFLAGS) -c src/ff1.c -o $@

src/ff3.o: src/ff3.c
	$(CC) $(CFLAGS) -c src/ff3.c -o $@

src/fpe_locl.o: src/fpe_locl.c
	$(CC) $(CFLAGS) -c src/fpe_locl.c -o $@

$(EXAMPLE_EXE): $(EXAMPLE_SRC) $(LIB)
	$(CC) $(CFLAGS) -Wl,-rpath=\$$ORIGIN $(EXAMPLE_SRC) -L. -lfpe -Isrc -o $@

clean:
	rm $(OBJS)

test:
	python test.py
