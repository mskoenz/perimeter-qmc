#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
#
# Author:  Mario S. Könz <mskoenz@gmx.net>
# Date:    12.06.2013 10:13:41 EDT
# File:    jackknife.py

import sys
from addon import *
import numpy as np
import glob

def jackknife(datain):
    data = np.array(datain[1:])
    sum_ = np.add.accumulate(data)[-1]
    
    N = float(len(data))
    mean = np.mean(data)
    
    #~ print("data mean: ", mean)
    
    fct = lambda x: -np.log(x)
    
    xjack = [(sum_ - d) / (N-1) for d in data]
    fjack = [fct(x) for x in xjack]
    
    #~ print(xjack[1:100])
    
    #~ print("xjack mean: ", np.mean(xjack))
    #~ print("xjack std:  ", np.std(xjack))
    #~ print("fjack mean: ", np.mean(fjack))
    #~ print("fjack std:  ", np.sqrt((N-1)) * np.std(fjack))
    return np.array([np.mean(fjack), np.sqrt((N-1)) * np.std(fjack)])
    
def gen_error(name):
    data = []
    ifs = open(name, "r")
    lines = ifs.readlines()
    ifs.close()
    
    for l in lines:
        line = l[:-1]
        if len(line) > 0:
            data.append(float(line))
            
    name = name[:-8] + "results.txt"
    
    ifs = open(name, "r")
    lines = ifs.readlines()
    ifs.close()
    
    return [jackknife(data), lines[1][:-1]]

def main():
    
    files = glob.glob("test_jack/*/*/*/mean.txt")
    files.sort()
    
    ofs = open("colres.txt", "w")
    
    name = files[0][:-8] + "results.txt"
    
    ifs = open(name, "r")
    lines = ifs.readlines()
    ifs.close()
    
    ofs.write(lines[0][:-1] + " mean error\n")
    
    sum_  = np.array([0, 0])
    
    for f in files:
        print(f)
        print("\033[2A")
        d = gen_error(f)
        err = d[0]
        sum_ = sum_ + err
        ofs.write(d[1] + " " + str(sum_[0]) + " " + str(sum_[1]) + "\n")
    ofs.close();
    
if __name__ == "__main__":
    print("jackknife.py")
    parameter.read(sys.argv)
    #~ main()
    
    data = np.array([1, 2, 3, 4, 5, 6])
    N = len(data)
    mean = np.mean(data)
    print(mean)
    print(np.std(data) / np.sqrt(N - 1))
    
    xjack = [-np.log((mean*N - d) / (N-1)) for d in data]
    
    print(np.mean(xjack))
    print(np.sqrt(N - 1) * np.std(xjack))
    
    
    
    
