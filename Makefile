CC=g++
CFLAGS=-g -Wall
OBJS=CarpManager.o main.o
TARGET=arpManager

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS)

CarpManager.o: CarpManager.cpp
	$(CC) -c -o CarpManager.o CarpManager.cpp

main.o : main.cpp
	$(CC) -c -o main.o main.cpp

clean: 
	rm -f *.o
	rm -f $(TARGET)
