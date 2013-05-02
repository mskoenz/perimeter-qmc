#!/usr/bin/python2.7
# -*- coding: cp1252 -*-
#
# Author:  Mario S. Könz <mskoenz@gmx.net>
# Date:    11.03.2012 11:43:40 CET
# File:    main.py

from sys import *
import thread
import time
import os
import glob


def run_cpp(mod):
    global counter2
    os.system("./examples/loop_to_braket_realisation "+mod)
    counter2 += 1

def is_float(element):
    try:
        float(element)
    except ValueError:
        return False
    return True

def sort_file(outf):
    of = file(outf, "r")
    lines = of.readlines()
    of.close()
    of = file(outf, "w")
    counter = 0;
    for l in reversed(lines):
        if l[0] == '-':
            break;
        else:
            counter += 1
    to_sort = lines[-counter:]
    tel = []
    for l in to_sort:
        tel.append([[float(s) for s in l.replace('/', ' ').split() if is_float(s)][2], l])
    
    of.writelines(lines[:-counter])
    start = sorted(tel)[0][0]
    for k in sorted(tel):
        if(int(k[0])>start):
            start = int(k[0])
            of.writelines("\n")
        of.writelines(k[1])
    
    of.close()
    
if __name__ == "__main__":
    counter1 = 0
    counter2 = 0
    
    #~ subfolder = "2x2/"
    #~ subfolder = "2x4/"
    #~ subfolder = "2x4/different1/"
    #~ subfolder = "4x4/"
    #~ subfolder = "6x6/"
    #~ subfolder = "6x6/different6/"
    subfolder = "2x4/different4/"
    #~ subfolder = "2x4/different3/"
    outf = subfolder[0:3] + ".txt"
    
    fls = glob.glob("./v2/" + subfolder + "*.txt")
    fls.sort()
    print(fls)
    
    of = file(outf, "a")
    of.write("---------------------------------------------------------------------------" + subfolder + "\n")
    of.close()
    print("start simulation")
    for i in range(len(fls)):
        for j in range(i, len(fls)):
            run_cpp(fls[i] + " " + fls[j] + " " + outf)
    #~ thread.start_new_thread(run_cpp, ("v2/6x6/02_v2.txt v2/6x6/02_v1.txt",))
    while(counter2 < 1):
        time.sleep(0.01)
    print("done simulation")
    #~ print '\n%-4s%-14s%-13s%-15s' % (1,2,3,4)
    
    sort_file(outf)
    
