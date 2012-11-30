#!/bin/bash

basedir=`dirname $0`

. $basedir/sastp.cfg

for i in 10 20 50 100 200 500 1000; do
    $SOLVER $@ $INSTANCEDIR/sastp_$i.prob $OUTPUTDIR/sastp_$i.sol
done
