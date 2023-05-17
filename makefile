
# client

client.o: src/client/client.h src/client/client.cpp
	g++ -c src/client/client.cpp -o client.o

main_client.o: src/client/main.cpp
	g++ -c src/client/main.cpp -o main_client.o -lspdlog

client.exe: client.o main_client.o
	g++ client.o main_client.o -o client.exe
	rm -f client.o main_client.o

# server

server.o: src/server/server.h src/server/server.cpp
	g++ -c src/server/server.cpp -o server.o

main_server.o: src/server/main.cpp
	g++ -c src/server/main.cpp -o main_server.o

server.exe: server.o main_server.o
	g++ server.o main_server.o -o server.exe
	rm -f server.o main_server.o


# general

clean:
	rm -f client.o server.o main_client.o main_server.o client.exe server.exe

all: client.exe server.exe

compile-all: client.exe server.exe

compile-all-and-run-client: client.exe server.exe
	./client.exe

compile-all-and-run-server: client.exe server.exe
	./server.exe

.PHONY: clean