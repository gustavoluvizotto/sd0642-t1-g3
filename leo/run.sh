#!/bin/sh

HOST=(x-men ciclope wolverine magneto hulk colossus tempestade demolidor venon vampira gambit elektra anjo justiceiro noturno)

MASTEREXEC="~/lm/master"
SERVEREXEC="~/lm/server"
CLIENTEXEC="~/lm/client"
LOGDIR="~/lm"

#Master
ssh -f ${HOST[14]} $MASTEREXEC

#Servers de nível 2
ssh -f ${HOST[13]} "$SERVEREXEC 14"
ssh -f ${HOST[12]} "$SERVEREXEC 13"

#Servers de nível 1
ssh -f ${HOST[11]} "$SERVEREXEC 12"
ssh -f ${HOST[10]} "$SERVEREXEC 11"
ssh -f ${HOST[9]} "$SERVEREXEC 10"
ssh -f ${HOST[8]} "$SERVEREXEC 9"

sleep 5
#Clients
ssh -f ${HOST[7]} "$CLIENTEXEC 8 > $LOGDIR/host8.log 2>&1"
ssh -f ${HOST[6]} "$CLIENTEXEC 7 > $LOGDIR/host7.log 2>&1"
ssh -f ${HOST[5]} "$CLIENTEXEC 6 > $LOGDIR/host6.log 2>&1"
ssh -f ${HOST[4]} "$CLIENTEXEC 5 > $LOGDIR/host5.log 2>&1"
ssh -f ${HOST[3]} "$CLIENTEXEC 4 > $LOGDIR/host4.log 2>&1"
ssh -f ${HOST[2]} "$CLIENTEXEC 3 > $LOGDIR/host3.log 2>&1"
ssh -f ${HOST[1]} "$CLIENTEXEC 2 > $LOGDIR/host2.log 2>&1"
ssh -f ${HOST[0]} "$CLIENTEXEC 1 > $LOGDIR/host1.log 2>&1"


