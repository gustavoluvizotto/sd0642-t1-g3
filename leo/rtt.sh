#!/bin/sh

HOST=(x-men ciclope wolverine magneto hulk colossus tempestade demolidor venon vampira gambit elektra anjo justiceiro noturno)

for i in `seq 0 14`; do ssh -f ${HOST[$i]} "lm/rtt_server" ; done
for i in `seq 0 14`; do ./rtt_client ${HOST[$i]} ; done

