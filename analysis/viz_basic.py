import matplotlib # RVMod
matplotlib.use('Agg') # RVMod
import matplotlib.pyplot as plt
import numpy as np

import os
import re # For regex
import argparse
import pdb


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

# LogType, Value
#   Note: Value can be an empty string or something more complicated, seperated by spaces
# Examples:
# "Queue-expansion" [Queue_name] [State]
# "LH-value" [State] [Improvement]
# "Map" [Mapstring]

class LoggerQueue:
    def __init__(self, name, theMap, valueName) -> None:
        shape = theMap.shape

        self.name = name
        self.theMap = theMap
        self.countMap = np.zeros(shape)
        self.valueMap = np.full(shape, np.nan) #np.zeros(shape)
        self.valueName = valueName

    def addCoord(self, aCoord, t) -> None:
        self.countMap[aCoord] += 1
        if np.isnan(self.valueMap[aCoord]):
            self.valueMap[aCoord] = t

    def plot(self, getAx, maxExpand=None, maxCount=None):
        self.compactMap()
        # if ax is None:
            # ax = plt.gca()
        # ax1 = plt.subplot(3, 2, rowNum)
        nCols = 2
        ax = getAx(nCols, 1)
        self.addMap(ax)
        maskedValueMap = np.ma.masked_where(np.isnan(self.valueMap), self.valueMap)

        vmax = maxExpand or maskedValueMap.max() # Becomes second only if maxExpand is None
        im = ax.imshow(maskedValueMap, norm=matplotlib.colors.Normalize(vmax=maskedValueMap.max()), 
                alpha=1, interpolation='none', origin='lower')
        ax.set_title("{}: {}".format(self.name, self.valueName))
        plt.colorbar(im, ax=ax)
        ax1 = ax

        # ax2 = plt.subplot(3, 2, rowNum+3)
        if np.nanmax(self.countMap) == 1:
            return
        ax = getAx(nCols, 2)
        self.addMap(ax)
        countMap = np.ma.masked_where(self.countMap==0, self.countMap)

        vmax = maxCount or countMap.max()
        im = ax.imshow(countMap, norm=matplotlib.colors.Normalize(vmin=1, vmax=vmax), 
                alpha=1, interpolation='none', origin='lower')
        # ax.imshow(countMap, norm=matplotlib.colors.LogNorm(vmin=1, vmax=vmax), 
        #         alpha=1, interpolation='none', origin='lower')
        ax.set_title("{}: Counts".format(self.name))
        plt.colorbar(im, ax=ax)
        ax2 = ax
        # return [ax1, ax2]
        
    def getNumPlots(self):
        return 2

    def addMap(self, ax):
        plt_map = np.ma.masked_where(self.theMap==0, self.theMap)
        ax.imshow(plt_map, cmap="binary", vmin=0, vmax=1,
            interpolation='none', origin='lower')

    def compactMap(self):
        # https://stackoverflow.com/questions/4808221/is-there-a-bounding-box-function-slice-with-non-zero-values-for-a-ndarray-in
        img = self.countMap
        rows = np.any(img, axis=1)
        cols = np.any(img, axis=0)
        ymin, ymax = np.where(rows)[0][[0, -1]]
        xmin, xmax = np.where(cols)[0][[0, -1]]
        pad = 5
        self.theMap = self.theMap[max(ymin-pad,0):ymax+pad+1,max(xmin-pad,0):xmax+pad+1]
        self.countMap = self.countMap[max(ymin-pad,0):ymax+pad+1,max(xmin-pad,0):xmax+pad+1]
        self.valueMap = self.valueMap[max(ymin-pad,0):ymax+pad+1, max(xmin-pad,0):xmax+pad+1]
        

class LoggerViz:
    def __init__(self) -> None:
        self.map = None
        self.loggerQueues = dict()
        self.expandNum = 0
        self.regexM = [re.compile("\((\d+),(\d+)"), re.compile("\((\d+),(\d+),(\d+)\)")][0]
        # self.regexM = re.compile("\((\d+),(\d+),(\d+)\)")

    # This function should be synced with SimpleLogger.cpp!
    def readLine(self, aLine) -> None:
        if (aLine.endswith(".map")):  # This is the map
            self.map = readMap(aLine)
            self.loggerQueues["Total"] = LoggerQueue("Total", self.map, "Time") # This is the aggregate total expansion map
        elif aLine[0] == "(":  # This is a state
            # self.expandNum += 1
            lineSplit = aLine.split(" ")
            if len(lineSplit) == 2:
                qName = lineSplit[1]
                coords = lineSplit[0]
                self.expandNum += 1
                val = self.expandNum
            elif len(lineSplit) == 3:
                qName = lineSplit[2]
                coords = lineSplit[0]
                val = float(lineSplit[1])

            m = self.regexM.match(coords)
            x, y = int(m.group(1)), int(m.group(2))

            if (qName not in self.loggerQueues):
                assert(self.map is not None)
                if len(lineSplit) == 2:
                    self.loggerQueues[qName] = LoggerQueue(qName, self.map, "Time")
                else:
                    self.loggerQueues[qName] = LoggerQueue(qName, self.map, "HFValue")

            self.loggerQueues[qName].addCoord((y,x), val)
            if len(lineSplit) == 2:
                self.loggerQueues["Total"].addCoord((y,x), val) # Always add to total
        

    def readFile(self, logfile) -> None:
        with open(logfile) as f:
            all_lines = f.readlines()

        for aLine in all_lines:
            self.readLine(aLine[:-1]) # Remove the \n at the end

    def createViz(self, outputFile) -> None:
        maxExpand = 0
        for aQueue in self.loggerQueues.values():
            maxExpand = max(maxExpand, aQueue.countMap.max())

        
        nrows = len(self.loggerQueues)
        maxCols = 2
        fig = plt.figure(figsize=(maxCols*6+4,nrows*6))

        for i, aLoggerQueue in enumerate(self.loggerQueues.values()):
            getAx = lambda col, ind: plt.subplot(nrows, col, col*i+ind)
            aLoggerQueue.plot(getAx, self.expandNum)
            # ax = plt.subplot(nrows, 1, i+1)
            # aLoggerQueue.plot(ax)
            

        # fig.subplots_adjust(left=0.15, right=0.85)
        # cbar_ax = fig.add_axes([0.02, 0.1, 0.07, 0.77])
        # fig.colorbar(plt.cm.ScalarMappable(norm=matplotlib.colors.Normalize(vmin=1, vmax=self.expandNum)),
        #                 cax=cbar_ax)
        # cbar_ax = fig.add_axes([0.87, 0.1, 0.07, 0.77])
        # fig.colorbar(plt.cm.ScalarMappable(norm=matplotlib.colors.Normalize(vmin=1, vmax=maxExpand)),
        #                 cax=cbar_ax) # LogNorm, Normalize
        # plt.tight_layout()
        plt.savefig(outputFile)
        

# def getIntsFromString(astring):
    # return a

def verifyLHData(logfile):
    with open(logfile) as f:
        occupancy_grid = f.readline()
        f.readline().rstrip()


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("filepath", help="log filepath", type=str) # Note: Positional is required and not --filepath
    # parser.add_argument("batch", help="running batch viz", type=str, required=True)
    args = parser.parse_args()

    logviz = LoggerViz()
    logviz.readFile(args.filepath)
    folder = os.path.dirname(args.filepath) + "/"  # Does not end with "/"
    outputFile = folder + "result.png"
    logviz.createViz(outputFile)