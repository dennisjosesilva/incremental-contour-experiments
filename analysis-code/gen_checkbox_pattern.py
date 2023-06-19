import sys

import numpy as np
import imageio as im

N = int(sys.argv[1])

X, Y = np.meshgrid(np.arange(N), np.arange(N))

f = (X%2 == Y%2)*255

if len(sys.argv) >= 3:
  im.imsave(sys.argv[2], f.astype(np.uint8))
else:
  im.imsave("checkboard.pgm",f.astype(np.uint8))