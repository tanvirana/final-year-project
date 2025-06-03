#with open("data.txt") as file:
#  data = file.read().splitlines()

#data = [int(x) for x in data]

#with open("weights.txt") as file:
#  weights = file.read().splitlines()

#weights = [int(w) for w in weights]

def convolve(weights, data):
  curOut = 0
  out = []
  prevData = []

  for t in range(len(data)):
  
    prevData.append(data[t])
    if len(prevData) > len(weights):
      prevData.pop(0)

    curOut = 0
    for i in range(len(weights)):
      if i < len(prevData):
        curOut += weights[i]*prevData[-i-1]
    
    out.append(curOut)
 
  return out
