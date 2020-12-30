#!/usr/bin/env python3

# https://www.geeksforgeeks.org/working-with-json-data-in-python/?ref=lbp
import json
import numpy as np
import sys, getopt

def main() :
  print ("Creating a Signal Graph\n")
  freq = 1000
  numSamples = 500
  fileName = "./test-data/signal.json"
  try:
    opts, args = getopt.getopt(sys.argv[1:],"hf:n:",["freq=","samples=", "file="])
  except getopt.GetoptError:
      print ('CreateSignal.py -f<freq> -n<samples> --file=<fileName>')
      sys.exit(2)
  print ("Options:", opts)
  for opt, arg in opts:
    if opt == '-h':
       print ('CreateSignal.py -f<freq> -n<samples>')
       sys.exit()
    elif opt in ("-f", "--freq"):
         freqArg = arg
         print ("Parsed Freq Argument", arg)
         if (int(freqArg)>0):
           freq = int(freqArg)
    elif opt in ("-n", "--samples"):
         numSamplesArg = arg
         print ("Parsed numSamples Argument", arg)
         if (int(numSamplesArg)>0):
           numSamples = int(numSamplesArg)
    elif opt in ("--file"):
         fileName = arg
         print ("fileName", fileName)
         
  
  amplitude = 100
  time = np.arange(1, numSamples, 1)
  amplitude = amplitude * np.sin(freq*time/(2*np.pi))
  print ("Freq", freq)
  print ("Samples", numSamples)
  
  data = []
  for i in range(len(time)):
    label = freq/numSamples * i
    dataPoint = {"label": label ,"x": time[i], "y" :amplitude[i]}
    data.append( dataPoint)
  
  jsonData = json.dumps(data, cls=NpEncoder )
  print("Data written to ",fileName)
  with open(fileName, "w") as outFile:
     outFile.write (jsonData)


class NpEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.integer):
            return int(obj)
        elif isinstance(obj, np.floating):
            return float(obj)
        elif isinstance(obj, np.ndarray):
            return obj.tolist()
        else:
            return super(NpEncoder, self).default(obj)


if __name__ == "__main__":
    main()