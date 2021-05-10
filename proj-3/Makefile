CC = g++
CXXFLAGS = -Wall -Werror -Wextra -pedantic -std=c++17 -g -fsanitize=address
LDFLAGS =  -fsanitize=address

CFLAGS = -Wall -g

SRC = 
OBJ = $(SRC:.cc=.o)
EXEC = main

all: main clean

main: main.o server.o client.o broadcast.o log.o
	$(CC) main.o client.o server.o broadcast.o log.o -o main 

main.o: main.cpp server.h client.h
	$(CC) -c main.cpp

server.o: server.cpp server.h broadcast.h
	$(CC) -c server.cpp

client.o: client.cpp client.h broadcast.h
	$(CC) -c client.cpp

broadcast.o: broadcast.cpp broadcast.h
	$(CC) -c broadcast.cpp

log.o: log.cpp log.h
	$(CC) -c log.cpp

clean:
	rm *.o
