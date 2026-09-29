CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

TARGET = pso
SRC    = main.c pso.c utils.c config.c args.c ini.c
OBJ    = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
