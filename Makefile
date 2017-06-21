all: server

server: 
		g++ -std=c++11 -o server server.cpp -Wall -lboost_system
clean: 
		-rm server
