#!/bin/sh

HOST=(x-men ciclope wolverine magneto hulk colossus tempestade demolidor venon vampira gambit elektra anjo justiceiro noturno)

for i in `seq 0 14`; do ssh -f ${HOST[$i]} "killall server master client >/dev/null 2>&1"; done

sleep 3
