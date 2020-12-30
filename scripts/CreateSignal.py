#!/usr/bin/env python3

# https://www.geeksforgeeks.org/working-with-json-data-in-python/?ref=lbp
import json
import numpy as np
import sys, getopt

def main() :
  print ("Creating a Signal Graph\n")
  freq1= 1000
  freq2= 1000
  numSamples = 500
  stepSize = 0.002

  fileName = "./test-data/signal.json"
  try:
    opts, args = getopt.getopt(sys.argv[1:],"hf:n:",["freq1=","samples=", "file=", "freq2=", "step="])
  except getopt.GetoptError:
      print ('CreateSignal.py --freq1=<freq> -n<samples> --file=<fileName> --freq2=<freq2> --step=<step>')
      sys.exit(2)
  print ("Options:", opts)
  for opt, arg in opts:
    if opt == '-h':
       print ('CreateSignal.py -f<freq> -n<samples>')
       sys.exit()
    elif opt in ("-f", "--freq1"):
         freqArg = arg
         print ("Parsed Freq Argument", arg)
         if (int(freqArg)>0):
           freq1 = int(freqArg)
    elif opt in ("-n", "--samples"):
         numSamplesArg = arg
         print ("Parsed numSamples Argument", arg)
         if int(numSamplesArg)>0 and int(numSamplesArg) < 10000:
           numSamples = int(numSamplesArg)
    elif opt in ("--file"):
         fileName = arg
         print ("fileName", fileName)
    elif opt in ("--freq2"):
         freq2= int(arg)
    elif (opt in("--step")):
         stepSize = arg.astype(np.int64)
  
  amplitude = 100
  bestStep = 1 / (2*max(freq1,freq2))
  if bestStep < stepSize:
      stepSize = bestStep

  time = np.arange(0, numSamples*stepSize, stepSize)
  # use precision 2 since we scale by 100.
  f1 = np.around(amplitude * np.sin(freq1*time/(2*np.pi)), 1)
  f2 = np.around(amplitude * np.sin(freq2*time/(2*np.pi)), 1)
  mix = np.around(f1+f2,1)
 
  time = np.around(time, 5)

  print ("Freq1", freq1)
  print ("Samples", numSamples)
  print ("Freq2", freq2)
  print ("step", stepSize)
  
  data = []
  for i in range(len(time)):
    label = np.around(freq1/numSamples * i, 1)
    dataPoint = {"label": label ,"x": time[i], "f1" :f1[i], "f2" : f2[i], "mix": mix[i]}
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