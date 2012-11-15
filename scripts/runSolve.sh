#!/bin/sh
#Generic Script to Solve SASTP Instances
SASTPPATH="/home1/yourID/whereYouWant"

#Path to your solver
EXECUTABLE="$SASTPPATH/YourSolverApp"

#Reduce for testing purposes, but final results need to be based on 30 runs
RUNS=30

#Location for your jobs, do not change
SOLVELOCATION="-l noX"


for SIZE in 10 20 50 100 200 500 1000
do
	INSTANCE=$SASTPPATH/wherever/sastp_${SIZE}.prob
	OUTPUTFOLDER=$SASTPPATH/wherever
	OUTPUTFILEBASE=extralog_${SIZE}

#flags for your application
	FLAGS="$INSTANCE"

	ARRAYFLAG="-t 1-$RUNS"

#get email notification if something with your jobs goes wrong
#add -m b and -m e to get mails at beginning and end of jobs
#beware, you can spam yourself heavily with this
	EMAILFLAG=" -m a -m s -M your@mail"

	JOBSCRIPT=$SASTPPATH/wherever/job_${SIZE}.sh

#workaround to get separate output and error logs for every run of an array job
	echo "#!/bin/bash" > $JOBSCRIPT
	echo "#$ -o $SASTPPATH/wherever/size$SIZE"-r'$TASK_ID'"o.log" >> $JOBSCRIPT
	echo "#$ -e $SASTPPATH/wherever/size$SIZE"-r'$TASK_ID'"e.log" >> $JOBSCRIPT
	echo "" >> $JOBSCRIPT
	cat $SASTPPATH/SolveArray.sh >> $JOBSCRIPT

#actually submit the job, you can modify the job name to your liking
	qsub -N "SASTP-$SIZE" $EMAILFLAG -r y $SOLVELOCATION $ARRAYFLAG\
	$JOBSCRIPT "$EXECUTABLE" "$FLAGS" $OUTPUTFILEBASE $OUTPUTFOLDER


done

echo "SOLVING JOBS SUBMITTED"


