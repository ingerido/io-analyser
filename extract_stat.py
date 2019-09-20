#!/usr/bin/python

import os
import sys

filepath = sys.argv[1]

if not os.path.isfile(filepath):
    print("File path {} does not exist. Exiting...".format(filepath))
    sys.exit()

totalIO = 0
concurrentIO = 0
IO_1K = 0
IO_1K_4K = 0
IO_4K_16K = 0
IO_16K = 0
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
            #if last[0][1] != fields[2] and linenum - last[0][3] < 256:
            if last[0][1] != fields[2]:
                concurrentIO = concurrentIO + 1
                if int(fields[4]) < 4096:
                    smallRandIO = smallRandIO + 1
                last[0][1] = fields[2]
                last[0][2] = fields[5]
                last[0][3] = linenum
        else:
            filelist.append(cur)
         
        if int(fields[4]) <= 1024:
            IO_1K = IO_1K + 1
        elif int(fields[4]) > 1024 and int(fields[4]) <= 4096:
            IO_1K_4K = IO_1K_4K + 1
        elif int(fields[4]) > 4096 and int(fields[4]) <= 16384:
            IO_4K_16K = IO_4K_16K + 1
        elif int(fields[4]) > 16384:
            IO_16K = IO_16K + 1

        totalIO = totalIO + 1
        linenum = linenum + 1

print "totalIO = ", totalIO 
print "concurrentIO = ", concurrentIO 
print "IO_1K = ", IO_1K
print "IO_1K_4K = ", IO_1K_4K
print "IO_4K_16K = ", IO_4K_16K
print "IO_16K = ", IO_16K
print "smallRandIO = ", smallRandIO

print "concurrentIO ", float(concurrentIO)/totalIO 
print "IO_1K ", float(IO_1K)/totalIO
print "IO_1K_4K ", float(IO_1K_4K)/totalIO
print "IO_4K_16K ", float(IO_4K_16K)/totalIO
print "IO_16K ", float(IO_16K)/totalIO
print "smallRandIO ", float(smallRandIO)/totalIO
