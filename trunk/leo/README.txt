Para executar esta solução (solução 2) siga estes passos:

1) make clean
2) make client server master
3) Abra o arquivo run.sh e configure as variáveis MASTEREXEC, SERVEREXEC e CLIENTEXEC para que apontem para o diretório onde estão os executáveis, e faça também a variável LOGDIR apontar para onde deseja que fiquem os logs.
4) execute ./killall.sh  para matar possíveis processos anteriores em execução
5) execute ./run.sh   Isso fará com que a solução seja totalmente executada, gerando logs de cada 1 dos hosts clientes em $LOGDIR/host[1..8].log

Analisando o log:

Dentro dos arquivos host[2..8].log estão presentes:
	- o vetor de 1000 posições ordenado
	- TTMC deste cliente (TTMC = tempo de transmissão master->client, conforme explicado no relatório)

Dentro do arquivo host1.log estão presentes:
	- a legenda com o significado de cada etiqueta utilizada no restante dos arquivos de log
	- todos os tempos importantes envolvidos na solução do problema


Exemplo de uso (cenário):

Copie todos os arquivos do projeto numa pasta qualquer, digamos "~/solucao2/"

~/solucao2$ ls
bubble.c        killall.sh  run.sh
Makefile        server.c    tp1.c
client.c        master.c    tp1.h
error.c         merge.c

~/solucao2$ make clean
~/solucao2$ make client server master

Configuração em run.sh:
MASTEREXEC="~/solucao2/master"
SERVEREXEC="~/solucao2/server"
CLIENTEXEC="~/solucao2/client"
LOGDIR="~/solucao2"   (sem a "/" no final)

~/solucao2$ ./killall.sh
~/solucao2$ ./run.sh

Após alguns segundos a execução termina e nada é impresso na tela. Tudo o que é gerado (vetor ordenado em cada cliente + tempos de execução) fica nos arquivos host[1..8].log


Obs: Todos os nós do cluster devem estar ativos para que o programa funcione. Se algum estiver down, pode ser que algum processo continue em execução. Para matá-lo, execute novamente ./killall.sh, que sai matando todos os meus processos em todos os nós da rede.

Obs2: Não execute nossos scripts como root, principalmente o ./killall.sh, senão ele pode sair matando outros processos que não os nossos.




