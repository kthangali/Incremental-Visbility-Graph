import matplotlib # RVMod
matplotlib.use('Agg') # RVMod
import matplotlib.pyplot as plt
import numpy as np

import torch
from torch.utils.data import Dataset

from collections import defaultdict
import os
import re # For regex
import argparse
import pdb


####################################################
###### Creating pytorch dataset ####################
class LocalHeuristicDataset(Dataset):
    def __init__(self, numpy_data_path, size, data_preprocessing):
        data = np.load(numpy_data_path)
        data = {key:val for key,val in data.items()} # Convert to dictionary so can modify afterwards

        dataSize = len(data["newHVal"])

        if size > dataSize:
            print("Requested size {} is greater than max size {}".format(size, dataSize))
        elif size == -1:
            pass
        else:
            index = np.random.choice(dataSize, size, replace=False)
            for aKey in data.keys(): #["x", "y", "newHVal", "mapIndex", "hvalIndex"]:
                if aKey not in ["map_obs", "map_hvals"]: # For all the other keys
                    data[aKey] = data[aKey][index]
        # pdb.set_trace()

        if data_preprocessing in ["improvement", "raw"]:
            pass
        else:
            raise KeyError("Unknown data_preprocessing value: {}".format(data_preprocessing))

        # data["map_obs"] = 1-data["map_obs"] # Switch, make sure obstacles are 1, freespace 0
        
        ## Pad maps and hvals
        # pdb.set_trace()
        data["map_obs"] = list(data["map_obs"])
        data["map_hvals"] = list(data["map_hvals"])
        for i in range(len(data["map_obs"])):
            data["map_obs"][i] = np.pad(data["map_obs"][i], data["k"], mode="constant", constant_values=1) # Add obstacles to border
        for i in range(len(data["map_hvals"])):
            data["map_hvals"][i] = np.pad(data["map_hvals"][i], data["k"], mode="edge") 

        # data["map_obs"] = np.pad(data["map_obs"], data["k"], mode="constant", constant_values=1) # Add obstacles to border
        # data["map_hvals"] = np.pad(data["map_hvals"], data["k"], mode="edge")  # Add -1 to border

        self.data = data
        # self.D = data["hvals"].shape[-1]
        self.data_preprocessing = data_preprocessing

    def __len__(self):
        return len(self.data["newHVal"])
        # return 32

    def __getitem__(self, idx):
        k = self.data["k"]
        # x = int(self.data["x"][idx]) + k # Add k due to padding
        # y = int(self.data["y"][idx]) + k
        state = self.data["state"][idx]
        x = int(state[0]) + k
        y = int(state[1]) + k

        mapIndex = self.data["mapIndex"][idx]
        obs = self.data["map_obs"][mapIndex][y-k:y+k+1, x-k:x+k+1]
        hvalIndex = self.data["hvalIndex"][idx]
        hvals = np.copy(self.data["map_hvals"][hvalIndex][y-k:y+k+1, x-k:x+k+1])

        target = self.data["newHVal"][idx]
        if self.data_preprocessing == "improvement":
            target -= self.data["map_hvals"][hvalIndex][y, x]
            hvals -= self.data["map_hvals"][hvalIndex][y, x]
            # pdb.set_trace()
        elif self.data_preprocessing == "raw":
            pass
        else:
            raise KeyError("Unknown data_preprocessing value: {}".format(self.data_preprocessing))
        # return self.data["obs"][idx], self.data["hvals"][idx], self.data["newHVal"][idx]
        if obs.shape != (2*k+1, 2*k+1):
            pdb.set_trace()
        # print(idx, obs, target)
        # pdb.set_trace()
        # print(idx)
        # pdb.set_trace()
        if len(state) == 3:
            # print(obs.shape, hvals.shape)
            obs = np.rot90(obs, int(state[2]))
            hvals = np.rot90(hvals, int(state[2]))
            # print(obs.shape, hvals.shape)
            # pdb.set_trace()

        return obs.copy(), hvals.copy(), target



###############################################################################################
###### Converting txt output from C++ to numpy arrays and saving to a file ####################
def load2DArray(all_lines, index):
    mapSize = all_lines[index].split(",")
    assert len(mapSize) == 2
    x, y = int(mapSize[0]), int(mapSize[1])

    ans = np.zeros((y,x))
    for i in range(0,y):
        aLine = all_lines[1+index+i].rstrip("\n")  # Remove end newline

        # pdb.set_trace()
        if (aLine[-1] == ","): # Remove trailing comma if there
            aLine = aLine[:-1]
        if (len(aLine.split(",")) != x):
            pdb.set_trace()
        ans[i] = np.fromstring(aLine, dtype=float, sep=",")
    return ans, index+y+1

# Read map
def readMap(mapfile):
    # mapfile = "../random-32-32-20.map"
    with open(mapfile) as f:
        line = f.readline()  # "type octile"

        line = f.readline()  # "height 32"
        height = int(line.split(' ')[1])

        line = f.readline()  # width 32
        width = int(line.split(' ')[1])

        line = f.readline()  # "map\n"
        assert(line == "map\n")

        mapdata = np.array([list(line.rstrip()) for line in f])

    mapdata.reshape((width,height))
    mapdata[mapdata == '.'] = 0
    mapdata[mapdata == '@'] = 1
    mapdata[mapdata == 'T'] = 1
    mapdata = mapdata.astype(int)
    return mapdata

## Old parsing function
# def parserLHLogfile(logfile):
#     with open(logfile) as f:
#         all_lines = f.readlines()
    
#     all_data = defaultdict(list)

#     index = 0
#     while index < len(all_lines)-1:
#         obs, index = load2DArray(all_lines, index)
#         hvals, index = load2DArray(all_lines, index)
#         newHVal = float(all_lines[index])
#         assert all_lines[index+1] == "\n"
#         index += 2

#         all_data["obs"].append(obs)
#         all_data["hvals"].append(hvals)
#         all_data["newHVal"].append(newHVal)
        
#     saveFolder = os.path.dirname(args.filepath)
#     np.savez("{}/data.npz".format(saveFolder), **all_data) # Note automatically stacks to numpy vectors

def parseSingleDataLine(aLineStr):
    stateStr, dataStr = aLineStr.split(")")
    state = [float(x) for x in stateStr[1:].split(",")]
    newHVal = float(dataStr)
    return state, newHVal


def parserLHLogfile(logfile, allData=None):
    with open(logfile) as f:
        all_lines = f.readlines()
    
    # all_data = defaultdict(list)
    if allData is None:
        allData = dict(
            k=5,
            mapNameToIndex = {},
            map_obs=[], # Map data
            hvalsToIndex = {},
            map_hvals=[], # 

            # x=[],
            # y=[],
            state = [],
            newHVal=[],
            mapIndex=[],
            hvalIndex=[],
        )

    mapfile = all_lines[0].rstrip()
    map_obs = readMap(mapfile)
    hvalsfile = all_lines[1].rstrip()
    with open(hvalsfile) as f:
        lines = f.readlines()
        map_hvals, _ = load2DArray(lines, 0)
    # map_hvals, index = load2DArray(all_lines, 1)
    assert(all_lines[2] == "\n")
    index = 3

    if mapfile not in allData["mapNameToIndex"].keys():
        allData["mapNameToIndex"][mapfile] = len(allData["map_obs"])
        allData["map_obs"].append(map_obs)
    mapIndex = allData["mapNameToIndex"][mapfile]

    if hvalsfile not in allData["hvalsToIndex"].keys():
        allData["hvalsToIndex"][hvalsfile] = len(allData["map_hvals"])
        allData["map_hvals"].append(map_hvals)
    hvalIndex = allData["hvalsToIndex"][hvalsfile]

    # regexM = re.compile("\((\d+),(\d+)\) (\d+.*)")  # e.g. "(20,9) 56.000000"
    while index < len(all_lines)-1:
        # m = regexM.match(all_lines[index])
        # x, y, newHVal = [float(ag) for ag in m.groups()]
        state, newHVal = parseSingleDataLine(all_lines[index])
        # allData["x"].append(x)
        # allData["y"].append(y)
        allData["state"].append(state)
        allData["newHVal"].append(newHVal)
        allData["mapIndex"].append(mapIndex)
        allData["hvalIndex"].append(hvalIndex)
        index += 1

    # pdb.set_trace()
    # saveFolder = os.path.dirname(logfile)
    # np.savez("{}/data.npz".format(saveFolder), **allData) # Note automatically stacks to numpy vectors
    return allData

def parseLHBatch(logfolder):
    # Each data points should have
    # State: (x,y)
    # Local observations: k, mapData -> mapName
    # Heuristic information: mapHvals -> heuristicIdentifiers
    allData = dict(
        k=5,
        mapNameToIndex={},
        map_obs=[], # Map data
        hvalsToIndex={},
        map_hvals=[], # hvals data

        # x=[],
        # y=[],
        state = [],
        newHVal=[],
        mapIndex=[],
        hvalIndex=[],
    )
    for file in os.listdir(logfolder):
        filename = os.fsdecode(file)
        if filename.endswith(".txt") and "lh" in filename:
            filepath = os.path.join(logfolder, filename)
            parserLHLogfile(filepath, allData) # This modifies allData
    
    del allData["mapNameToIndex"]
    del allData["hvalsToIndex"]
    # for aKey in allData.keys():
        # allData[aKey] = np.array(allData[aKey])
    # pdb.set_trace()
    np.savez_compressed("{}/data.npz".format(logfolder), **allData) # Note automatically stacks to numpy vectors



################################################
############# Main function ####################

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("filepath", help="log filepath", type=str) # Note: Positional is required and not --filepath
    # parser.add_argument("batch", help="running batch viz", type=str, required=True)
    args = parser.parse_args()

    # parserLHLogfile(args.filepath)
    parseLHBatch(args.filepath)