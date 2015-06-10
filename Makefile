all:
	g++ server.cpp -lpthread -o server
	g++ client.cpp -o client
