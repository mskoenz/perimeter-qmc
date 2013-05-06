#!/usr/bin/python3.2
# -*- coding: utf-8 -*-
#
# Author:  Mario S. Könz <mskoenz@gmx.net>
# Date:    03.05.2013 08:52:43 EDT
# File:    clean.py

import sys
import glob

def clean(name):
    print("start cleaning: ", name)
    if_ = open(name, "r")
    lin = if_.readlines()
    if_.close()
    numbers = []
    for l in lin[:-1]:
        numbers.append(l.split()[-1])
    
    of_ = open(name+"c.txt", "w")
    for n in numbers:
        of_.write(n)
        of_.write("\n")
    of_.close()
    print("done cleaning: ", name)
    
if __name__ == "__main__":
    print("clean.py")
    files = glob.glob('./loop_config/*.txt')
    
    if len(files) == 0:
        print("no files for cleaning")
    else:
        print(len(files), " files for cleaning")
        
    
    for f in files:
        clean(f)
    
