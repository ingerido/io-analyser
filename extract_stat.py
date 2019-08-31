#!/usr/bin/python

import os
import sys

filepath = sys.argv[1]

if not os.path.isfile(filepath):
    print("File path {} does not exist. Exiting...".format(filepath))
    sys.exit()

totalIO = 0
concurrentIO = 0
smallIO = 0
smallRandIO = 0
filelist = []
linenum = 0

with open(filepath) as f:
    for line in f:
        # skip the first line
        if linenum == 0:
            linenum = 1
            continue

        # split line into fields[0]: filename, fields[1]: PID, fields[2]: TID,
        # fields[3]: operation, fields[4]: iosize, fields[5]: offset
        fields = line.split(" | ")

        cur = [fields[0], fields[2], fields[5], linenum]
        last = [item for item in filelist if item[0] == fields[0]]
        if len(last) == 1:
            if last[0][1] != fields[2] and linenum - last[0][3] < 70:
            #if last[0][1] != fields[2]:
                concurrentIO = concurrentIO + 1
                if int(fields[4]) < 10240:
                    smallRandIO = smallRandIO + 1
                last[0][1] = fields[2]
                last[0][2] = fields[5]
                last[0][3] = linenum
        else:
            filelist.append(cur)
         
        if int(fields[4]) < 10240:
            smallIO = smallIO + 1

        totalIO = totalIO + 1
        linenum = linenum + 1

print "totalIO = ", totalIO 
print "concurrentIO = ", concurrentIO 
print "smallIO = ", smallIO 
print "smallRandIO = ", smallRandIO

print "concurrentIO ", float(concurrentIO)/totalIO, "%" 
print "smallIO ", float(smallIO)/totalIO, "%" 
print "smallRandIO ", float(smallRandIO)/totalIO, "%" 
