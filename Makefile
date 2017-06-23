all: server client

server: 
		g++ -std=c++11 -o server server.cpp -Wall -lboost_system
client:
		g++ -std=c++11 -o client client.cpp -Wall -lboost_system -lpthread
clean: 
		-rm server client
