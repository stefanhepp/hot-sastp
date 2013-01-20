#!/usr/bin/env python
import os,sys
import time
import subprocess 
import csv
from shlex import split as splitsh

directory = ""
outputDirectory = ""

def runCommand(program, arguments, problem, iteration, uniqueID):
    outputFile = problem.strip(".prob")
    command = program + " " + arguments + " "+directory+problem + " " +outputDirectory+uniqueID+outputFile+"_"+str(iteration+1)+".sol"
    os.system(command + ">> LogFile"+uniqueID+".csv" )
    print "Solving : " + program + " " + arguments + " "+ problem + " " +uniqueID+outputFile+"_"+str(iteration+1)+".sol" 

#### return the list of problems which are to be run ###########
def getProblems(dirname,lis):
    for filename in os.listdir(dirname):
        if filename.endswith("prob"):
            lis.append(filename)
    lis.sort()


##### run each problem in this configuration for 30 times ############
def runNTimes(program, arguments, inputFile, times, uniqueID):
    for item in inputFile:
         if "sastp1000" in item or "sastp500" in item: 
             times = 5
         else : 
             times = 30
         for i in range(0, times):
             runCommand(program, arguments, item, i, uniqueID);

############ run different configurations for  n times ##################

def runDifConf(program, configurationList, inputFile, times):
    i = 1
    for item in configurationList: 
        runNTimes(program, item.strip("\n"), inputFile, times,"conf"+str(i) )
        i = i + 1 

        
#################  create the Latex table for each logFile #############
import numpy


'''
\begin{table}[b!]
  \vspace{-6mm}%
  \caption{Computed values using the nearest tour neighborhood}
  \label{tab:NearestTour}
  \setlength{\tabcolsep}{1.4mm}
  \centering
  \begin{tabular}{lrrrrrr}
   \multirow{2}{*}{\bfseries Problem} &
      \multicolumn{2}{c}{\bfseries Local Search, Next} &
      \multicolumn{2}{c}{\bfseries VND, Next} &
      \multicolumn{2}{c}{\bfseries Grasp (VND), Next} \\
    &
    \bfseries Satis. &
    \bfseries Time &
    \bfseries Satis. &
    \bfseries Time &
    \bfseries Satis. &
    \bfseries Time 
    \\\hline
sastp10   & 43.2979 & 0.00 & 43.2979 & 0.00 & 43.2979 & 0.02 \\
sastp20   & 67.2554 & 0.00 & 69.9204 & 0.00 & 72.4877 & 0.14 \\
sastp50   & 195.385 & 0.04 & 208.364 & 0.07 & 210.068 & 1.43 \\
sastp100  & 465.156 & 0.43 & 492.373 & 0.52 & 499.048 & 21.44 \\
sastp200  & 904.097 & 1.82 & 960.046 & 3.01 & 968.095 & 180.44 \\
sastp500  & 2265.16 & 3.82 & 2608.02 & 80.29 & 2612.06 & 657.99 \\
sastp1000 & 4474.67 & 1.38 & 5451.09 & 667.66 & 5449.17 & 1141.64 
    \\\hline
  \end{tabular}

\end{table}

'''
def ComputeForOneProblem(fileWrite, problemName, lines):
    maximumSatisfaction = 0.0
    avgTime = 1.0
    bestSatisfaction = 0.0
    standardDev = []
    noRuns = 0
    for index in lines: 
        spl = index.split(",")
        #print spl[0], problemName
        if problemName == spl[0]: 
            noRuns = noRuns + 1
            avgTime = avgTime + float(spl[2])
            standardDev.append(float(spl[1]))
            if float(spl[1]) > bestSatisfaction : 
                bestSatisfaction = float(spl[1])
    #print standardDev
    deviation = numpy.std(standardDev)
    if noRuns == 0 : 
        avg = -1
    else: 
        avg = avgTime/noRuns
    fileWrite.write(problemName+" & "+ str(bestSatisfaction)+ " & "+ str(deviation)+ " & " +str(avg) + "\n")
    #print problemName+" & "+ str(bestSatisfaction)+ " & "+ str(deviation)+ " & " +str(avg) + "\n"

def CreateLatexTable(logfile):
    log = open(logfile, "r")
    logLines = log.readlines();
    logLines.sort()
    stdev = []
    problems = ["sastp10","sastp20","sastp50", "sastp100", "sastp200", "sastp500", "sastp1000"]

    fout = open("Latex_from_"+logfile,"a")
    for line in (problems): 
        ComputeForOneProblem(fout,line, logLines)
        #if splitted[0].endswith("sastp10") : 
        #    stdev.append(float(splitted[1]))
    #print numpy.std(stdev)
    #print numpy.average(stdev)
    #print logLines

'''
    with open(logfile,"rb") as csvfile : 
        reader = csv.reader(csvfile, delimiter=",", quotechar = '|')
    for row in reader: 
        print row
'''


if __name__ == '__main__':
    listOfProblems=[]
    if len( sys.argv) < 4 : 
        print "runProblems <executable> <inputProblemsDirectory> <outputDirectory> <configurationFile> "
        exit(1)
    executable = sys.argv[1]
    directory = sys.argv[2]
    outputDirectory = sys.argv[3]
    configurations = sys.argv[4]
    confList=[]
    
    outputFile = ""
    
    confFile = open(configurations, "r")
    configurations = confFile.readlines()
    
    problemList =[]
    getProblems(directory, problemList)
     
    #instead of 1 place the number of runs of this configuration
    runDifConf(executable, configurations, problemList, 30)

    for x in range(0, len(configurations)):
        CreateLatexTable("LogFileconf"+str(x+1)+".csv")
    
    

