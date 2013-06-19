#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
#
# Author:  Mario S. Könz <mskoenz@gmx.net>
# Date:    06.06.2013 20:49:45 EDT
# File:    parameter.py

import sys
from color import *
from helper import *
import os

#------------------- parameter class ------------------- 
class parameter_class(dict):
    def __init__(self):
        super(dict, self).__init__()
        pass

    def read(self, argv):
        pas = False
        self["arg"] = []
        for i in range(1, len(argv)):
            w = argv[i]
            if w[0] == '-' and len(w) > 1:
                if i + 1 < len(argv) and argv[i+1][0] != '-':
                    self[w[1:]] = to_number(argv[i+1])
                    pas = True
                else:
                    self[w[1:]] = "not_set"
            else:
                if pas:
                    pas = False
                else:
                    self["arg"].append(w)
        
    def contains(self, flag):
        return flag in self.keys()

parameter = parameter_class()

#------------------- parameter action ------------------- 
def bash_if(flag, action):
    if parameter.contains(flag):
        if type(action) == type(" "): #normal bash cmd
            bash(action)
        else: #fct call
            CYAN("called function")
            action()
    return 0

def bash(cmd):
    CYAN(cmd)
    os.system(cmd)
