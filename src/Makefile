CC := gcc
CFLAGS := -g -Wall -Wextra -I./include

# Files to be complied
TARGET := main
OBJ := main.o dll.o mcb.o yamm.o

$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	-rm $(TARGET) $(OBJ)