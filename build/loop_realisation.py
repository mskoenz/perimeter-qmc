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

if __name__ == "__main__":
    counter1 = 0
    counter2 = 0
    
    subfolder = "6x6/different6/"
    outf = "out.txt"
    
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
    
