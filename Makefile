CC=gcc
EXECUTABLE=bloom
LIBS=-levent -lcrypto
SOURCES=bloom.c

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) -ggdb -o $(EXECUTABLE) $(SOURCES) $(LIBS)
