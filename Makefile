CC=gcc
CFLAGS=-O2
EXECUTABLE=bloom
LIBS=-levent -lcrypto
STATIC_LIBS=-ldl
SOURCES=bloom.c

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $(EXECUTABLE) $(SOURCES) $(LIBS)

static: $(EXECUTABLE).static

$(EXECUTABLE).static: $(SOURCES)
	$(CC) -static -static-libgcc $(CFLAGS) -o $(EXECUTABLE).static $(SOURCES) $(LIBS) $(STATIC_LIBS)

clean:
	rm -f $(EXECUTABLE) $(EXECUTABLE).static
