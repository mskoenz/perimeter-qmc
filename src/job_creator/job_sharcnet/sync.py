#!/usr/bin/python2.7
# -*- coding: utf-8 -*-
#
# Author:  Mario S. Könz <mskoenz@gmx.net>
# Date:    06.06.2013 14:01:12 EDT
# File:    min_plot.py

import sys

from addon import *

import os
import shutil

def exists(name):
    return os.access(name, os.R_OK)

#------------------- helper -------------------
def is_list(obj):
    return type(obj) == type([])

def str_elegant(nr):
    res = ""
    if nr < 10:
        res += "0"
    if nr < 100:
        res += "0"
    
    res += str(nr)

    return res;

loc = ord('A') - 1
idx_list = []

def recursion(path, dirs, fct, intermed = False):
    global loc
    global idx_list
    loc += 1
    if is_list(dirs[0]) and len(dirs[0]) == 2 and type(dirs[0][0]) != type(dirs[0][1]):
        for i in range(len(dirs)):
            idx_list.append(dirs[i][0]);
            if intermed:
                fct(path + "/" + chr(loc) +"-"+ str_elegant(dirs[i][0]))
            recursion(path + "/" + chr(loc) +"-"+ str_elegant(dirs[i][0]), dirs[i][1], fct, intermed)
            idx_list.pop()
        
        loc -= 1
        return
    
    
    for dir_ in dirs:
        idx_list.append(dir_);
        fct(path + "/"+ chr(loc)+"-" + str_elegant(dir_))
        idx_list.pop()
    loc -= 1


#------------------- make folders -------------------
def mkfolder(path_dir):
    if not exists(path_dir):
        os.mkdir(path_dir)
        GREEN(path_dir + " created")
    else:
        YELLOW(path_dir + " exists already")


def mkfolders(path, dirs):
    recursion(path, dirs, mkfolder, True)

#------------------- remove folders -------------------
def rmfolder(path_dir):
    if exists(path_dir):
        shutil.rmtree(path_dir)
        
        RED(path_dir + " deleted")
    else:
        YELLOW(path_dir + " already deleted")
    
def rmfolders(path, dirs):
    for dir_ in dirs:
        rmfolder(path + str_elegant(dir_))

#------------------- copy to folders ------------------- 
def cp_to_folder(data, path_dir):
    if exists(path_dir):
        if exists(data):
            shutil.copy(data, path_dir)
            GREEN(data + " copied to " + path_dir)
        else:
            RED(data + " not found")
    else:
        YELLOW(path_dir + " not found")
    
def cp_to_folders(data, path, dirs):
    if not is_list(data):
        data = [data]
    
    for d in data:
        fct = lambda path_dir: cp_to_folder(d, path_dir)
        recursion(path, dirs, fct)

#------------------- progress ------------------- 
def fancy_progress_bar(p):
    size = 50;
    
    bars = int(p * size);
    bar = "<"

    for i in range(bars):
        bar += "|"
    for i in range(bars, size):
        bar += " "
    bar += "> "
    end = str(int(p*100)) + "%"
    
    if(p < .33):
        RED(bar + NONE_ + end)
    elif(p < .66):
        YELLOW(bar + NONE_ + end)
    elif(p == 1):
        CYAN(bar + NONE_ + end)
    else:
        GREEN(bar + NONE_ + end)

#------------------- state files ------------------- 
queue = 0
done = 0
run = 0
N = 10
total_p = 0
slowest_job = "00:00:00";
def read_state_files(path, dirs):
    global queue
    global done
    global run
    global total_p
    global slowest_job
    queue = 0
    done = 0
    run = 0
    total_p = 0
    slowest_job = "00:00:00";
    
    def read_state_file(path_dir):
        global queue
        global done
        global run
        global total_p
        global slowest_job
        if exists(path_dir + "/state.txt"):
            ofs = open(path_dir + "/state.txt", "r")
            ll = ofs.readlines()
            ofs.close()
            tel = {}
            for l in ll:
                l = l.split();
                tel[l[0]] = to_int(l[1])
            
            total_p += tel["p"];
            if(tel["p"] == 1):
                done += 1
            else:
                run += 1
                if "a" in parameter.keys():
                    CYAN("job in " + path_dir + " is done in " + REDB_ + tel["t"], "")
                    fancy_progress_bar(tel["p"])
                
                if tel["t"] > slowest_job:
                    slowest_job = tel["t"];
        else:
            queue += 1 
    
    recursion(path, dirs, read_state_file)
    
    N = queue + done + run
    
    if queue:
        REDB([str(queue), "all"][N == queue] + " job" + ["s",""][queue == 1] + " are still in queue or failed")
    if run:
        YELLOWB([str(run), "all"][N == run] + " job" + ["s",""][run == 1] + " are running")
    if done:
        GREENB([str(done), "all"][N == done] + " job" + ["s",""][done == 1] + " are done")
    
    CYAN("last running job in " + path + "/* is done in " + REDB_ + slowest_job, "")
    
    fancy_progress_bar(total_p / (queue + run + done))
#------------------- collect results -------------------
def collect_results(path, dirs):
    bash("cp " + path + "/B-000/results.txt " + path + "/colres.txt")
    
    fct = lambda path_dir: bash("tail -2 " + path_dir + "/results.txt >> " + path + "/colres.txt")
    
    recursion(path, dirs, fct)
        

#------------------- bash files ------------------- 
def write_bash_file(path_dir):
    args = parameter["arg"];
    ofs = open(path_dir + "/bash_in.txt", "w")
    
    ofs.write(parameter["bash"] + " ")
    
    for i in range(len(args)):
        arg = args[i]
        if is_list(arg):
            for a in arg:
                ofs.write("-" + a + " " + str(idx_list[i]) + " ")
        else:
            ofs.write("-" + arg + " " + str(idx_list[i]) + " ")
    
    ofs.write("\n")
    ofs.close()
    CYAN("bash file nr " + str(idx_list) + " written")

def write_bash_files(path, dirs):
    recursion(path, dirs, write_bash_file)

#------------------- prog start -------------------
def launch_program(path_prog):
    os.system(path_prog + " & ")
    #~ os.system("sqsub -q serial -r 3d -o run.log " + path_prog)
    GREEN("job: " + path_prog + " submitted")
    #~ os.system("sleep 0.1")
    
def launch_programs(prog, path, dirs):
    for dir_ in dirs:
        launch_program(path + str_elegant(dir_) + "/" + prog)

def kill_programs(id0, dirs):
    for dir_ in dirs:
        bash("sqkill " + str(id0 + dir_))

#=================== main ===================
def main():
    CLRSCR("")
    
    parameter.read(sys.argv)
    
    gl = lambda x:[j for j in range(0, x+1)]
    
    stage0 = [[i, gl(i)] for i in range(2, 31, 2)]
    
    parameter["arg"] = [["L", "H"], "g"]
    parameter["bash"] = "-shift 16x16_shift.txt -mult 10"
    
    root = "sim_cluster";
    
    files = ["../../../build/examples/sim_playground", "../../../examples/30x30_shift.txt"]
    
    if "sel" in parameter.keys():
        ll = eval(parameter["sel"])
        for idx in ll:
            for s in stage0:
                if s[0] == idx:
                    stage0 = s[1]
                    break;
        temp = stage0;
        parameter["sel"] = root
        for idx in reversed(ll):
            global loc
            loc += 1
            parameter["sel"] += "/" + chr(loc) + "-" + str_elegant(idx)
            temp = [[idx, temp]]
        
        root = parameter["sel"]
    
    bash_if("j", "sqjobs")
    bash_if("l", "cat run.log")
    bash_if("s", lambda:read_state_files(root, stage0))
    
    bash_if("c", lambda:collect_results(root, stage0))
    bash_if("p", "./plot.py " + root + "/colres.txt -p -x 6 -y 7 -o " + root + "/plot.png")
    
    bash_if("clean", lambda:( rmfolder(root)
                            , bash("rm *.log")
                            , bash("rm *.png")
                            , bash("rm *.txt")
                            ))
    bash_if("make", lambda: ( mkfolder(root)
                            , mkfolders(root, stage0)
                            , cp_to_folders(files, root, stage0)
                            , write_bash_files(root, stage0)
                            ))
    #~ bash_if("run", lambda:launch_programs("sim_playground", "sim_array/sim", range(N)))
    #~ 
    #~ bash_if("kill", lambda:kill_programs(15251921, range(N)))
    #~ 

if __name__ == "__main__":
    print("min_plot.py")
    main();
