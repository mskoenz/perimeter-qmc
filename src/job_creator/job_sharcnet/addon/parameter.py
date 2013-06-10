#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
#
# Author:  Mario S. Könz <mskoenz@gmx.net>
# Date:    06.06.2013 20:49:45 EDT
# File:    parameter.py

import sys
from color import *
import os

#------------------- helper ------------------- 
def to_int(string):
    try:
        res = int(string)
        return res
    except:
        pass
    try:
        res = float(string)
        return res
    except:
        return string

#------------------- parameter class ------------------- 
class parameter_class(dict):
    def __init__(self):
        self._mydict = {} 

    def read(self, argv):
        for i in range(1, len(argv)):
            w = argv[i]
            if w[0] == '-' and len(w) > 1:
                if i + 1 < len(argv) and argv[i+1][0] != '-':
                    parameter[w[1:]] = to_int(argv[i+1])
                else:
                    parameter[w[1:]] = "not_set"
        
        
parameter = parameter_class()

#------------------- parameter action ------------------- 
def bash_if(flag, action):
    if flag in parameter.keys():
        if type(action) == type(" "): #normal bash cmd
            bash(action)
        else: #fct call
            CYAN("called function")
            action()
    return 0

def bash(cmd):
    CYAN(cmd)
    os.system(cmd)
