#!/usr/bin/python3.2
# -*- coding: utf-8 -*-
#
# Author:  Mario S. Könz <mskoenz@gmx.net>
# Date:    03.05.2013 09:30:27 EDT
# File:    config_collider.py

from sys import *
import time
import os
import glob
import random

def run_cpp(mod):
    global counter2
    os.system("./examples/config_collider "+mod)
    counter2 += 1

def is_float(element):
    try:
        float(element)
    except ValueError:
        return False
    return True

dic = {}
same = [0]
def hypothesis(nrl):
        
    l1 = nrl[0]
    l2 = nrl[1]
    #~ cmean = nrl[2]
    crossA = nrl[3]
    cross2 = nrl[4]
    andA = nrl[5]
    and1 = nrl[6]
    xorA = nrl[7]
    xor1 = nrl[8]
    ws1H = nrl[9]
    ws1L = nrl[10]
    ws2H = nrl[11]
    ws2L = nrl[12]
    idw = nrl[13]
    xr = nrl[14]
    xr2 = nrl[15]
    an = nrl[16]
    an2 = nrl[17]
    poss = nrl[18]
    maxx = nrl[19]
    nr1 = nrl[20]
    nr2 = nrl[21]
    
    k = (l1, l2, crossA, cross2, andA, and1, xorA, xor1, ws1H, ws1L, ws2H, ws2L, idw, xr, xr2, an, an2)
    
    if k in dic.keys():
        if dic[k] != nrl[2]:
            print("fail: ", k, "   ", nrl)
            same[0] += 1
    else:
        dic[k] = nrl[2]
    
    
    return maxx/2 + min(l1, l2) - (max(l1, l2) if crossA == (l1+l2) else 0)

def sort_file(outf):
    of = open(outf, "r")
    lines = of.readlines()
    of.close()
    of = open(outf, "w")
    counter = 0;
    for l in reversed(lines):
        if l[0] == '-':
            break;
        else:
            counter += 1
    to_sort = lines[-counter:]
    tel = []
    success = [0, 0]
    for l in to_sort:
        nrl = [float(s) for s in l.replace('/', ' ').split() if is_float(s)];
        success[0] += 1
        success[1] += 1
        if nrl[2] != hypothesis(nrl):
            success[1] -= 1
            l = "hyp: " + str(hypothesis(nrl)) + l
        tel.append([nrl[2], l])
    
    of.writelines(lines[:-counter])
    start = sorted(tel)[0][0]
    for k in sorted(tel):
        if(int(k[0])>start):
            start = int(k[0])
            of.writelines("\n")
        of.writelines(k[1])
    
    of.close()
    print("hypothesis was ", int(success[1] * 1000 / success[0])/10, "% successfull")
    print(same[0], " configs where the same")

def generate_sample_file(numbers, size, subfolder):

    of = open("./loop_config/" + subfolder + "rnd.fast", "w")
    for i in range(size):
        i = random.randint(0, len(numbers) - 1)
        of.write(numbers[i])
        of.write("\n")
        numbers.pop(i)
    
    of.close()
    
    
if __name__ == "__main__":
    counter1 = 0
    counter2 = 0
    #~ subfolder = "2x2/"
    #~ subfolder = "4x2/"
    #~ subfolder = "8x2/"
    subfolder = "4x4/"
    
    outf = subfolder[0:3] + ".txt"
    
    fls = glob.glob("./loop_config/" + subfolder + "*.txt")
    fls.sort()
    
    if len(argv) >= 2:
        if argv[1] == "fast":
            fls = glob.glob("./loop_config/" + subfolder + "*.fast")
    
    print(fls)
    
    of = open(outf, "w")
    of.write("---------------------------------------------------------------------------" + subfolder + "\n")
    of.close()
    
    print("load numbers")
    
    numbers = []
    for f in fls:
        if_ = open(f, "r")
        lin = if_.readlines()
        if_.close()
        for l in lin:
            numbers.append(l[:-1])
    print(len(numbers))
    
    if len(argv) >= 3:
        if argv[1] == "rand":
            print("build random list")
            generate_sample_file(numbers, int(argv[2]), subfolder)
    else:
        print("start simulation")
        print(subfolder[0] + " " + subfolder[2] + " " + numbers[0] + " " + numbers[1] + " " + outf)
        print(int(len(numbers) * (len(numbers) - 1) / 2), " lines expected")
        for i in range(len(numbers)):
            for j in range(i, len(numbers)):
                run_cpp(subfolder[0] + " " + subfolder[2] + " " + numbers[i] + " " + numbers[j] + " " + outf)
            #~ thread.start_new_thread(run_cpp, ("v2/6x6/02_v2.txt v2/6x6/02_v1.txt",))
        #~ while(counter2 < 1):
            #~ time.sleep(0.01)
        print("done simulation")
        
        sort_file(outf)
        
