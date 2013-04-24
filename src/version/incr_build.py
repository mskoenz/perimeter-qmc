#!/usr/bin/python3.2
# -*- coding: cp1252 -*-
#
# Author:  Mario S. Könz <mskoenz@gmx.net>
# Date:    16.07.2012 10:53:25 CEST
# File:    incr_build.py

from sys import *

f = file("../src/version/build.hpp", "r")
g = f.readlines()
f.close()

l = g[0].split()
nr = int(l[-1]) + 1

l[-1] = str(nr)

g = " ".join(l)

f = file("../src/version/build.hpp", "w")
f.write(g)
f.write("\n")
f.close()
