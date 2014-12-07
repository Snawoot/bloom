CC=gcc
EXECUTABLE=bloom
LIBS=-levent -lcrypto
SOURCES=bloom.c

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) -ggdb -o $(EXECUTABLE) $(SOURCES) $(LIBS)

static: $(EXECUTABLE).static

$(EXECUTABLE).static: $(SOURCES)
	$(CC) -static -static-libgcc -ggdb -o $(EXECUTABLE).static $(SOURCES) $(LIBS)

clean:
	rm -f $(EXECUTABLE) $(EXECUTABLE).static
