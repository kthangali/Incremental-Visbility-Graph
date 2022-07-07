import matplotlib # RVMod
matplotlib.use('Agg') # RVMod
import matplotlib.pyplot as plt
import numpy as np

from collections import defaultdict
import os
import re # For regex
import argparse
import pdb


import itertools


def advanced2Dindexinto(array2D, indices):
    """Input: array2D (W,H), indices: (N,2)
        Output: (N) values indexing into 2D array"""
    return array2D[tuple(zip(indices.T))]

def convertOccupanyToStr(anInt):
    if anInt == 0:  # 0 denotes free space
        return "."
    return "T"  # 1 denotes obstacles

def getStateStr(aState):
    return "{},{}".format(aState[0], aState[1])

def createRandomScenarios():
    fileName = "random_test"
    D = 256  # Map dimension size
    obstacleProb = 0.2
    randmap = np.random.binomial(1, obstacleProb, size=(D,D))  # 1 denotes obstacles

    ns = 1000  # Number of samples
    validInds = np.argwhere(randmap == 0)
    if ns > len(validInds):
        raise RuntimeError("Not enough valid indices: {}, Wanted: {}".format(len(validInds), ns))
    # sampleInds = np.random.choice(len(validInds), ns, replace=False)
    # sampleInds = itertools.permutations(sampleInds, 2)
    startInds = np.random.choice(len(validInds), ns, replace=True)
    goalInds = np.random.choice(len(validInds), ns, replace=True)
    sampleInds = zip(startInds, goalInds)

    ### Write map file
    with open('{}.map'.format(fileName), 'w') as f:
        f.write("type octile\n")
        f.write("height {}\nwidth {}\n".format(D, D))
        f.write("map\n")
        for aRow in randmap:
            f.write("".join(map(convertOccupanyToStr, aRow)) + "\n")  # Converts to string, joins, adds newline
    

    ### Write scen file
    with open('{}.scen'.format(fileName), 'w') as f:
        f.write("simple_version\n")
        f.write("{}.map\n".format(fileName))
        for startInd, goalInd in sampleInds:
            f.write("{} {}\n".format(getStateStr(validInds[startInd]), getStateStr(validInds[goalInd])))
    
    
    



if __name__ == "__main__":
    # parser = argparse.ArgumentParser()
    # parser.add_argument("filepath", help="log filepath", type=str) # Note: Positional is required and not --filepath
    # # parser.add_argument("batch", help="running batch viz", type=str, required=True)
    # args = parser.parse_args()
    createRandomScenarios()