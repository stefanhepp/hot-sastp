
#!/bin/bash

basedir=`dirname $0`

. $basedir/sastp.cfg


$SOLVER -a 1 -s 2 -n 0 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/ls.oneopt.best.csv
$SOLVER -a 1 -s 1 -n 0 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/ls.oneopt.next.csv
$SOLVER -a 1 -s 0 -n 0 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/ls.oneopt.rand.csv

$SOLVER -a 1 -s 2 -n 1 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/ls.edgeopt.best.csv
$SOLVER -a 1 -s 1 -n 1 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/ls.edgeopt.next.csv
$SOLVER -a 1 -s 0 -n 1 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/ls.edgeopt.rand.csv

$SOLVER -a 1 -s 2 -n 2 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/ls.methodopt.best.csv
$SOLVER -a 1 -s 1 -n 2 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/ls.methodopt.next.csv
$SOLVER -a 1 -s 0 -n 2 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/ls.methodopt.rand.csv

$SOLVER -a 2 -s 2 -n 2 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/vnd.best.csv
$SOLVER -a 2 -s 1 -n 2 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/vnd.next.csv
# $SOLVER -a 2 -s 0 -n 2 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/vnd.rand.csv

$SOLVER -a 3 -s 2 -n 0 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/grasp.ls.oneopt.best.csv
$SOLVER -a 3 -s 1 -n 0 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/grasp.ls.oneopt.next.csv
# $SOLVER -a 3 -s 0 -n 0 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/grasp.ls.oneopt.rand.csv

$SOLVER -a 3 -s 2 -n 1 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/grasp.ls.edgeopt.best.csv
$SOLVER -a 3 -s 1 -n 1 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/grasp.ls.edgeopt.next.csv
# $SOLVER -a 3 -s 0 -n 1 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/grasp.ls.edgeopt.rand.csv

$SOLVER -a 3 -s 2 -n 2 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/grasp.ls.methodopt.best.csv
$SOLVER -a 3 -s 1 -n 2 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/grasp.ls.methodopt.next.csv
# $SOLVER -a 3 -s 0 -n 2 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/grasp.ls.methodopt.rand.csv

$SOLVER -a 4 -s 2 -n 2 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/grasp.vnd.best.csv
$SOLVER -a 4 -s 1 -n 2 -cp $INSTANCEDIR/sastp1000.prob $OUTPUTDIR/sastp1000.sol > $REPORTDIR/graphs/grasp.vnd.next.csv




