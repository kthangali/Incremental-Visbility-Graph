from collections import deque, defaultdict
import time
import numpy as np

# https://dbader.org/blog/python-context-managers-and-with-statement
# https://stackoverflow.com/questions/5109507/pass-argument-to-enter
# https://realpython.com/python-timer/


class CustomTimer:
    def __init__(self):
        self.stack = deque()
        self.time_dict = defaultdict(list)

    def __enter__(self):
        return self

    def __call__(self, aKey):
        self.stack.append((aKey, time.time()))
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        if len(self.stack):
            aKey, aTime = self.stack.pop()
            dif = time.time() - aTime
            self.time_dict[aKey].append(dif)

    # Returns sum of times per key
    def getTimes(self, aKey=None):
        if aKey is None:
            for aKey in self.time_dict:
                self.time_dict[aKey] = np.sum(self.time_dict[aKey])
            return self.time_dict  # Dictionary
        else:
            return np.sum(self.time_dict[aKey])   # Single value

def exampleUse():
    # Example use
    t = CustomTimer()
    for i in range(10):
        with t("A1"):
            time.sleep(0.05)
        with t("B1"):
            time.sleep(1)
    values = t.getTimes()

    print(values)
    # access A1 and B1 times

if __name__ == "__main__":
    # timeDataloader()
    exampleUse()