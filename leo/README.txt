Para executar esta versão da solução siga estes passos:

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


