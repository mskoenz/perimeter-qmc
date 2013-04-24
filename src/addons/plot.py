#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
#
# Author:  Mario S. Könz <mskoenz@gmx.net>
# Date:    23.04.2013 13:45:54 EDT
# File:    plot.py

import sys
import os
import copy
import pylab

def analyze_argv(arg):
    if len(arg) < 2:
        print("please name the input-file or -h")
        return -1
    
    fname = arg[1]
    
    if fname == "-h":
        print()
        print()
        print("this tool provides plot-functions for input-files")
        print("-------------------------------------------------")
        print("call the python script as follows:")
        print("./plot.py file.txt -options")
        print("the options are:")
        print("-l: show the labels")
        
    
    if not os.access(fname, os.R_OK): #file is readable
        print("this input-file is not readable")
        return -1
    
    #=================== read file ===================
    if_ = open(fname, "r")
    raw_lines = if_.readlines()
    if_.close()
    labels = raw_lines[0][:-1].split(' ') #get labels
    labels.append("index[-]") #add extra label
    n_labels = len(labels)
    
    lines = [x[:-1] for x in raw_lines[1:] if x[0] != '-'] #remove comments and labels
    
    data = [[] for i in labels];

    index = 0
    for i in range(len(lines)):
        lines[i] = lines[i].split(' ') + [str(i)]
        index += 1
        for j in range(n_labels):
            data[j].append(lines[i][j])
    
    if len(arg) < 3:
        print("please add additional options (for help: ./plot.py -h)")
        return -1
    
    option0 = arg[2]
    
    ### +---------------------------------------------------+
    ### |                   info                            |
    ### +---------------------------------------------------+
    
    if option0 == "-l":
        print("the possible labels are: ")
        for i in range(n_labels):
            print(i, ":", labels[i])
        return 0
    
    if option0 == "-cat":
        print("file content: ")
        for l in raw_lines:
            print(l[:-1])
        return 0
    
    if option0 == "-catc":
        print("file content clean: ")
        print(raw_lines[0][:-1])
        for l in lines:
            print(' '.join(l))
        return 0
    
    ### +---------------------------------------------------+
    ### |                   merge                           |
    ### +---------------------------------------------------+
    if option0 == "-m":
        if len(arg) < 4:
            print("please name the second input-file")
            return -1
        
        f2name = arg[3]
        if f2name == fname:
            print("the second file must be different form the first")
            return -1
        if not os.access(f2name, os.R_OK): #file is readable
            print("the second input-file is not readable")
            return -1
        
        #=================== read second file ===================
        if_ = open(f2name, "r")
        raw_lines2 = if_.readlines()
        if_.close()
        labels2 = raw_lines2[0][:-1].split(' ') #get labels
        labels2.append("index[-]") #add extra label
        n_labels2 = len(labels2)
        
        #------------------- in case some labels form file 2 have the same name as in file 1 ------------------- 
        for i in range(n_labels2):
            if labels2[i] in labels:
                p = labels2[i].find("[")
                if p != -1:
                    labels2[i] = labels2[i][:p]+"2"+labels2[i][p:]
                else:
                    labels2[i] += "2"
        
        
        lines2 = [x[:-1] for x in raw_lines2[1:] if x[0] != '-'] #remove comments and labels
        
        data2 = [[] for i in labels2];
        
        index = 0
        for i in range(len(lines2)):
            lines2[i] = lines2[i].split(' ') + [str(i)]
            for j in range(n_labels2):
                data2[j].append(lines2[i][j])
            index += 1
        
        if len(arg) < 5:
            print("please use -sync folloed by the label(s) of file 1 and then the correspondent of file 2")
            return -1
        
        if arg[4] == "-sync":
            llist = arg[5:7]
            if llist == []:
                print("choose alternating labels from file 1 and 2")
                print("possible labels for file 1:")
                for i in range(n_labels):
                    print(i,":",labels[i])
                print("possible labels for file 2:")
                for i in range(n_labels2):
                    print(i,":",labels2[i])
                return -1
            if llist[0] in labels:
                ind1 = labels.index(llist[0])
            else:
                try:
                    ind1 = int(llist[0])
                    if ind1 >= len(labels) or ind1 < 0:
                        print(llist[0], "is too big or small")
                        return -1
                except:
                    print(llist[0], "is no label of file 1")
                    return -1
            
            if llist[1] in labels2:
                ind2 = labels2.index(llist[1])
            else:
                try:
                    ind2 = int(llist[1])
                    if ind2 >= len(labels2) or ind2 < 0:
                        print(llist[1], "is too big or small")
                        return -1
                except:
                    print(llist[1], "is no label of file 1")
                    return -1

            lines3 = []
            
            for i in range(len(lines)):
                for j in range(len(lines2)):
                    if lines[i][ind1] == lines2[j][ind2]:
                        vector1 = lines[i][:-1] #index is never written
                        vector2 = copy.deepcopy(lines2[i]);
                        if labels2[ind2] != labels2[-1]:
                            vector2.pop(ind2)
                        vector2.pop(-1)
                        lines3.append(vector1 + vector2)
                        break
            vector1 = labels[:-1] #index is never written
            vector2 = copy.deepcopy(labels2);
            if labels2[ind2] != labels2[-1]:
                vector2.pop(ind2)
            vector2.pop(-1)
            
            labels3 = vector1 + vector2
            
            if len(arg) < 9:
                print("please use -o and a target-filename")
                return -1
            if arg[7] == "-o":
                ofname = arg[8]
                if not os.access(fname, os.W_OK): #file is readable
                    print("this output-file is not writeable")
                    return -1
                
                of_ = open(ofname, "w")
                
                of_.write(' '.join(labels3))
                of_.write("\n")
                for l in lines3:
                    v = ' '.join(l)
                    of_.write(v)
                    of_.write("\n")
                of_.close()
    
    ### +---------------------------------------------------+
    ### |                   kill single rows                |
    ### +---------------------------------------------------+
    if option0 == "-k":
        if len(arg) < 4:
            print("please name the label you want removed")
            print("possible labels are:")
            for i in range(n_labels - 1): #index cannot be killed, since its not written anyway
                print(i,":",labels[i])
            return -1
        
        if len(labels) == 2:
            print("you cannot kill the last entry")
            return -1
        
        if arg[3] in labels:
            ind = labels.index(arg[3])
        else:
            try:
                ind = int(arg[3])
                if ind >= len(labels) - 1 or ind < 0:
                    print(arg[3], "is too big or small")
                    return -1
            except:
                print(arg[3], "is no label of file 1")
                return -1
        
        if len(arg) < 6:
            if len(arg) == 4:
                arg.append("-o")
                arg.append(fname)
        
        if arg[4] == "-o":
            ofname = arg[5]
            if not os.access(fname, os.W_OK): #file is readable
                print("this output-file is not writeable")
                return -1
                
            of_ = open(ofname, "w")
            
            
            labels.pop(ind)
            labels.pop(-1)
            
            of_.write(' '.join(labels))
            of_.write("\n")
            for l in lines:
                l.pop(ind)
                l.pop(-1) #index is never written
                v = ' '.join(l)
                of_.write(v)
                of_.write("\n")
            of_.close()
    
    ### +---------------------------------------------------+
    ### |                   plot                            |
    ### +---------------------------------------------------+
    if option0 == "-p":
        llist = arg[3:]
        
        if "-x" not in llist:
            print("you need an option for the x-Axis: -x label")
        else:
            xarg = llist.index("-x")
            try:
                x = llist[xarg + 1]
                if x in labels:
                    xind = labels.index(x)
                else:
                    try:
                        xind = int(x)
                        if xind >= len(labels) or xind < 0:
                            print(x, "is too big or small")
                            return -1
                    except:
                        print(x, "is no label of file 1")
                        return -1
                
            except:
                print("you need to specify a x-label")
                print("possible labels are:")
                for i in range(n_labels):
                    print(i,":",labels[i])
                return -1
            
        if "-y" not in llist:
            print("you need an option for the y-Axis: -y label(s)")
        else:
            xarg = llist.index("-y")
            try:
                y = llist[xarg + 1]
                if y in labels:
                    yind = labels.index(y)
                else:
                    try:
                        yind = int(y)
                        if yind >= len(labels) or yind < 0:
                            print(y, "is too big or small")
                            return -1
                    except:
                        print(y, "is no label of file 1")
                        return -1
                
            except:
                print("you need to specify a y-label")
                print("possible labels are:")
                for i in range(n_labels):
                    print(i,":",labels[i])
                return -1
        
        pylab.xlabel(labels[xind])
        pylab.title(fname)
        pylab.ylabel(labels[yind])
        pylab.grid(True)
        pylab.plot(data[xind], data[yind], "r-")
        #marker = ".", linestyle = "-", color = "r"
        pylab.show()
    return 0

if __name__ == "__main__":
    arg = analyze_argv(sys.argv)
    
