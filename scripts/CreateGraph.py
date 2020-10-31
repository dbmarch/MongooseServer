#!/usr/bin/env python3

# https://www.geeksforgeeks.org/working-with-json-data-in-python/?ref=lbp
import json
import numpy as np

print ("Creating a Graph\n")

time = np.arange(0, 100, 0.1)
amplitude = np.sin(time) * 100

data = []
for i in range(len(time)):
  dataPoint = {"x": time[i], "y" :amplitude[i]}
  data.append( dataPoint)

jsonData = json.dumps(data)

# this script is executed by our back end code.
# It will run from the main folder.   The test-data subfolder is below 
outFile = open ("test-data/data.json", "w")
outFile.write (jsonData)
outFile.close()

# data = [
#   {"x": 1, "y" :3},
#   {"x": 2, "y" :3},
#   {"x": 3, "y" :3},
#   {"x": 4, "y" :3},
#   {"x": 5, "y" :3},
#   {"x": 6, "y" :3},
# ]


# class Point:
#   def __init__(self, x=0, y=0):
#     """ Create a new point """
#     self.x = x
#     self.y = y
   
#   def __str__(self):
#     return "{0} \"x\" : {1}, \"y\" : {2} {3}".format("{", self.x, self.y, "}")


