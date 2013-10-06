VERSION = 0.01
CC      = /usr/bin/gcc
CFLAGS  = -Wall -g -DVERSION=\"$(VERSION)\"
LDFLAGS = -lX11

OBJ = keystats.o
OUTPUT = keystats

all: $(OBJ)
        $(CC) $(CFLAGS) -o $(keystats) $(OBJ) $(LDFLAGS)

%.o: %.c
        $(CC) $(CFLAGS) -c $<