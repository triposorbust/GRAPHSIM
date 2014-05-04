#!/usr/bin/env python

from subprocess import Popen
import datetime as dt

import string
import random
import sys
import os

BINARY="graphsim"
ITERATIONS = 50

def suffix(n=6):
    chars = string.uppercase + string.lowercase + string.digits
    return "".join([random.choice(chars) for _ in xrange(n)])

def main(graph, spec, path, times):
    if not os.path.exists(BINARY):
        print("No binary found for \"{0}\".".format(BINARY))
        exit(-1)
    if not os.path.isdir(path):
        os.mkdir(path)
    dts = dt.datetime.now().strftime("%Y%m%d-%H%M")
    opath = os.path.join(path, dts)
    os.mkdir(opath)
    for _ in xrange(times):
        filename = os.path.join(opath, "{0}-{1}.csv".format(dts, suffix()))
        args = [graph, spec, filename, ITERATIONS]
        cmd = "./graphsim {0} {1} -o {2} -n {3} -t 2".format(*args)
        p = Popen(cmd, shell=True)
        p.wait()
    return None

if __name__ == "__main__":
    if len(sys.argv) != 5:
        print("Usage\n% python deploy.py <graph> <spec> <path> <times>");
        exit(-1)
    graph = sys.argv[1]
    spec  = sys.argv[2]
    path  = sys.argv[3]
    times = int(sys.argv[4])
    main(graph, spec, path, times)
