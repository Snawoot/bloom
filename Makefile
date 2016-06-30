CC=gcc
CFLAGS=-O2
EXECUTABLE=bloom
LIBS=-levent -lcrypto
STATIC_LIBS=-ldl
OBJ=bloom.o sighandlers.o bf_storage.o util.o bf_ops.o commands.o handler.o bf_hash.o
DEPS=defines.h globals.h sighandlers.h bf_storage.h bf_types.h util.h bf_ops.h commands.h handler.h bf_hash.h

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
