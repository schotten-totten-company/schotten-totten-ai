BUILDDIR = build
TARGET = $(BUILDDIR)/schotten
LIBS = -lm
CC = gcc
CFLAGS = -g  -march=native #-O3 #-O5 #-Wall


.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, $(BUILDDIR)/%.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

$(BUILDDIR)/%.o: %.c $(HEADERS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f $(BUILDDIR)/*.o
	-rm -f $(TARGET) 