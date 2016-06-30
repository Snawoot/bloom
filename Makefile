CC=gcc
CFLAGS=-O2 -I/usr/local/include
EXECUTABLE=bloom
LIBS=-levent -lcrypto -L/usr/local/lib
OBJ=bloom.o sighandlers.o bf_storage.o util.o bf_ops.o commands.o handler.o bf_hash.o
DEPS=globals.h sighandlers.h bf_storage.h bf_types.h util.h bf_ops.h commands.h handler.h bf_hash.h
PREFIX=/usr/local

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

static: $(EXECUTABLE).static

$(EXECUTABLE).static: $(OBJ)
	$(CC) -static -static-libgcc $(CFLAGS) -o $@ $(OBJ) $(LIBS)

bloom.o: bloom.c $(DEPS) globals.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLE) $(EXECUTABLE).static *.o

install: $(EXECUTABLE)
	install -m 775 $(EXECUTABLE) $(PREFIX)/bin
