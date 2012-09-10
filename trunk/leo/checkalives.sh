#!/bin/bash

HOSTS=(x-men ciclope wolverine magneto hulk colossus tempestade demolidor venon vampira gambit elektra anjo justiceiro noturno)
LOG=~/lm/deadhosts.log

while ((1)); do for i in `seq 0 14`; do ping -w 1 -c 1 -q ${HOSTS[$i]} >/dev/null 2>&1; if [ $? != 0 ]; then echo Morto em $(date) - ${HOSTS[$i]} >> $LOG 2>&1; fi; done; sleep 300; done

