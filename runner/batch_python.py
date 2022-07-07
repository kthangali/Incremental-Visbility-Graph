from datetime import datetime
import enum # For printing current datetime
import subprocess # For executing c++ executable
import numpy as np

import argparse
import re  # Regex
import sys
import pdb
import pandas as pd
from os.path import exists

class ScenRunner:
    """Class for running a single scen file"""
    def __init__(self, batchFolder, mapfile, scenfile, timeLimit, numExpToRun):
        self.batchFolder = batchFolder
        self.mapfile = mapfile
        self.scenfile = scenfile
        self.timeLimit = timeLimit
        self.numExpToRun = numExpToRun

        # self.regexM = re.compile("(\d+),(\d+) (\d+),(\d+)")

    def runSingle(self, start, goal, count):
        command = "../build_release/batch_h_runner --map={}".format(self.mapfile)
        command += " --batchFolder={}".format(self.batchFolder)
        command += " --timeout={}".format(self.timeLimit)
        command += " --start={} --goal={}".format(start, goal)
        # print(command.split(" "))
        subprocess.run(command.split(" "), check=True) # True if want failure error

    def runSceneFile(self):
        with open(self.scenfile, 'r') as f:
            alllines = f.readlines()
        assert(alllines[0] == "simple_version\n")

        mapname = self.mapfile.split("/")[-1]
        assert(alllines[1] == "{}\n".format(mapname))

        for i, aLine in enumerate(alllines[2:]):
            start, goal = aLine.rstrip().split(" ")  # Removes end \n then splits
            self.runSingle(start, goal, i)
            if  self.numExpToRun is not None and i >= self.numExpToRun:
                break

    

if __name__ == "__main__":
    # parser = argparse.ArgumentParser()
    # parser.add_argument("map", help="map to run", type=str) # Note: Positional is required, make not position by changing to --yKey
    # args = parser.parse_args()
    scenRunnerArgs = dict(
        batchFolder = "/home/rishi/Desktop/CMU/Research/search-zoo/logs/testBatch/",
        # mapfile = "/home/rishi/Desktop/CMU/Research/search-zoo/data/arena_v1_4.map",
        # scenfile = "/home/rishi/Desktop/CMU/Research/search-zoo/data/arena_v1_4.scen",
        mapfile = "/home/rishi/Desktop/CMU/Research/search-zoo/ml/random_test.map",
        scenfile = "/home/rishi/Desktop/CMU/Research/search-zoo/ml/random_test.scen",
        timeLimit = 30,  # In seconds
        numExpToRun = [None, 100][0],  # Number of experiments to run
    )
    br = ScenRunner(**scenRunnerArgs)
    br.runSceneFile()