#!/bin/bash
#this script will run on the execution servers
#command to execute
COMMAND=$1

#arguments for your program
ARGUMENTS=$2

#base name for output file, will be appended with run number
#useful when your program writes extra files
OUTFILENAME=$3

#where to put the outputfile
OUTPUTFOLDER=$4

SGEID=$JOB_ID

echo "running SolveJob on $HOSTNAME"

RUN=$SGE_TASK_ID

OUTPUTFILE="$OUTPUTFOLDER/$OUTFILENAME-r$RUN.log"
OUTPUTFILEFLAG="--outputFile=$OUTPUTFILE"

#Hint: always supply your program with the next three arguments in some form
#sgeid*100+runNr is a good way to set the seed (store it somewhere for debugging), 
#knowing the hostname where your program was run can become relevant during debugging
SGEIDFLAG="--sgeID=$SGEID"
RUNNRFLAG="--runNr=$RUN"
HOSTNAMEFLAG="--hostname=$HOSTNAME"


echo "command: $COMMAND $ARGUMENTS $OUTPUTFILEFLAG $RUNNRFLAG $SGEIDFLAG $HOSTNAMEFLAG"

# uncomment this line to execute your command after you have tested that the scripts
# actually do what you want them to do
#	       $COMMAND $ARGUMENTS $OUTPUTFILEFLAG $RUNNRFLAG --sgeID=$SGEID --hostname=$HOSTNAME




echo "Solving Job FINISH"
