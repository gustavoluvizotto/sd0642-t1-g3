all: client server master


client:
	gcc -Wall -Wextra -o client client.c tp1.c merge.c bubble.c error.c -lpthread

server:
	gcc -Wall -Wextra -o server server.c tp1.c merge.c bubble.c error.c -lpthread

master:
	gcc -Wall -Wextra -o master master.c tp1.c merge.c bubble.c error.c -lpthread

clean:
	rm *.o client server master
