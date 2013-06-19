#!/usr/bin/python3.2
# -*- coding: utf-8 -*-
#
# Author:  Mario S. Könz <mskoenz@gmx.net>
# Date:    12.06.2013 10:31:13 EDT
# File:    helper.py

import os

#------------------- helper ------------------- 
def exists(name):
    return os.access(name, os.R_OK)

def to_number(string):
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

def is_list(obj):
    return type(obj) == type([])

def is_int(obj):
    return type(obj) == type(int(0))

def is_float(obj):
    return type(obj) == type(float(0))

def is_number(obj):
    return is_int(obj) or is_float(obj)



def drange(start, end, step):
    return [step * i for i in range(int(start/step), int(end/step))]
