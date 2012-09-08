all: client server master


client:
	gcc -o client client.c tp1.c merge.c bubble.c -lpthread

server:
	gcc -o server server.c tp1.c merge.c bubble.c -lpthread

master:
	gcc -o master master.c tp1.c merge.c bubble.c -lpthread

clean:
	rm *.o client server master
