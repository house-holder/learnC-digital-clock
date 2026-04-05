PROJECT = digital-clock
SRC = $(PROJECT).c

# prefer no edits below
TARGET = bin/$(PROJECT)
CFLAGS = -Wall -Wextra -g
LDLIBS = -lraylib

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p bin
	gcc $(CFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -rf bin
