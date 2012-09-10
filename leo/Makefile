client:
	gcc -o client client.c tp1.c merge.c bubble.c error.c -lpthread

server:
	gcc -o server server.c tp1.c merge.c bubble.c error.c -lpthread

master:
	gcc -o master master.c tp1.c merge.c bubble.c error.c -lpthread

rtt_client:
	gcc -o rtt_client rtt_client.c tp1.c

rtt_server:
	gcc -o rtt_server rtt_server.c tp1.c

clean:
	rm client server master rtt_client rtt_server

run:
	./run.sh

kill:
	./killall.sh


