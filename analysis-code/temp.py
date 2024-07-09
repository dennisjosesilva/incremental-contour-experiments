import os 
from os.path import isfile, join
import numpy as np

seed = 42
rgen = np.random.RandomState(seed)

directory = "../dataset-icdar/test"
l = [f for f in os.listdir(directory) if isfile(join(directory, f))]

l = np.array(sorted(l))
idx = rgen.permutation(len(l))
l = l[idx[:50]]

for f in l:
  print(f)