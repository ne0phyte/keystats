VERSION = 0.01
CC = gcc

TARGET = keystats
CFLAGS  = -g -Wall -DVERSION=\"$(VERSION)\"
LDFLAGS = -lX11

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
