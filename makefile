default: server client

server: server.c 
	gcc server.c -o server

run_server: server
	./server

client: client.c 
	gcc client.c -o client

run_client: client
	./client

clean:
	rm server client

