Para executar o trabalho, os seguintes passos devem ser feitos:

1) Copie todos os arquivos em um diretório, como por exemplo /home/grupo03sd/
2) Execute o compile.sh, com o comando ./compile.sh
   Este comando irá compilar todos os arquivos .c e gerar seus respectivos executáveis.
3) Abra 15 terminais (ou 15 abas de um terminal). Cada uma dessas abas deve ser feito o acesso ao cluster de IP 143.107.181.10 e porta 22222
4) Em cada uma das 15 abas, abra cada um dos 15 nós do cluster usando o comando ssh, como por exemplo ssh venon. Cada nó deve ser aberto na 
seguinte sequencia para maior facilidade na execução:
	1º:  x-men
	2º:  ciclope
	3º:  wolverine
	4º:  magneto
	5º:  hulk
	6º:  colossus
	7º:  tempestade
	8º:  demolidor
	9º:  venon
	10º: vampira
	11º: gambit
	12º: elektra
	13º: anjo
	14º: justiceiro
	15º: noturno
5) Em cada um dos nós aberto, acesse o diretório onde se encontram os arquivos do trabalho.
6) A partir do nó 15 (noturno), execute código-à-código até chegar no client1, começando por serverp, ou seja:
	(noturno)	./serverp
	(justiceiro)	./servers2
	(anjo)		./servers1
	(elektra)	./server4
	(gambit)	./server3
	(vampira)	./server2
	(venon)		./server1
	(demolidor)	./client8
	(tempestade)	./client7
	(colossus)	./client6
	(hulk)		./client5
	(magneto)	./client4
	(wolverine)	./client3
	(ciclope)	./client2
	(x-men)  	./client1
	
Os resultados obtidos são explicados em RESULTADOS.txt (apenas seu significado) e melhores discutidos no relatório 1.
