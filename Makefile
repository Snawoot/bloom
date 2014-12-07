CC=gcc
EXECUTABLE=bloom
LIBS=-levent -lcrypto
SOURCES=bloom.c

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) -o $(EXECUTABLE) $(SOURCES) $(LIBS)
