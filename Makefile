CC=gcc
CFLAGS=-O2
EXECUTABLE=bloom
LIBS=-levent -lcrypto
STATIC_LIBS=-ldl
OBJ=bloom.o sighandlers.o storage.o util.o ops.o commands.o handler.o hash.o
DEPS=defines.h globals.h sighandlers.h storage.h types.h util.h ops.h commands.h handler.h hash.h

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

static: $(EXECUTABLE).static

$(EXECUTABLE).static: $(SOURCES)
	$(CC) -static -static-libgcc $(CFLAGS) -o $@ $^ $(LIBS) $(STATIC_LIBS)

bloom.o: bloom.c $(DEPS) globals.c
	$(CC) -c $(CFLAGS) -o $@ $<

%.o: %.c $(DEPS)
	$(CC) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLE) $(EXECUTABLE).static *.o
