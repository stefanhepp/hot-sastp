#!/usr/bin/env python
import os,sys
import csv

with open("VNDBest.csv","rb") as csvfile : 
    reader = csv.reader(csvfile, delimiter=",", quotechar = '|')
    for row in reader: 
        print ', '.join(row)

