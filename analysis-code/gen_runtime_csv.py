#!/usr/bin/python

import subprocess
import re 

import time
import pandas as pd 

import morphotreepy as mt
import imageio as im

from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter

def main(OUTPUT_FILE_NAME= "runtime/runtime_9.csv",
         IMAGE_BASE_PATH = "../programs/img",
         START_INCREMENTAL = True):

  # ===================================================================
  # Program Calls 
  # ===================================================================
  class ProgramCalls:
    def __init__(self, image):
      self.image_ = image 

    @property
    def image(self):
      return self.image_

    @property
    def perf_incr_contour(self):
      return f"../programs/build/perf_incr_contour"

    @property
    def perf_non_incr_contour(self):
      return f"../programs/build/perf_non_incr_contour"

    def _run_program(self, program):
      full_program_call = f"{program} {IMAGE_BASE_PATH}/{self.image_}" 
      print(full_program_call)

      p = subprocess.Popen(full_program_call, stdout=subprocess.PIPE, shell=True)

      (output, err) = p.communicate()

      p_status = p.wait()
      p_output = output.decode()

      print(p_output)

      runtime_regex = re.compile(r"time elapsed:\s+(?P<runtime>\d+)")
      mo = runtime_regex.search(p_output)
      runtime_str = mo.group("runtime")  
      return int(runtime_str)

    def run_perf_incr_contour(self):
      return self._run_program(self.perf_incr_contour)

    def run_perf_non_incr_contour(self):
      return self._run_program(self.perf_non_incr_contour)


  # ===================================================================
  # Max-tree number of nodes
  # ===================================================================
  def max_tree_number_of_nodes(f):
    # Read and setup image
    fv = mt.UI8Vector(f.ravel())
    domain = mt.Box.fromSize(mt.UI32Point(f.shape[1], f.shape[0]))

    # build max-tree
    adj = mt.Adjacency4C(domain)
    tree = mt.UI8buildMaxTree(fv, adj)
    return tree.numberOfNodes

  # ===================================================================
  # Read image
  # ===================================================================
  def read_image(image):
    return im.imread(f"{IMAGE_BASE_PATH}/{image}")

  # ===================================================================
  # main
  # ===================================================================
  RUNTIME_PROGRAM_CALLS = [
    ProgramCalls("2.pgm"), 
    ProgramCalls("3.pgm"),
    ProgramCalls("barb.pgm"),
    ProgramCalls("bird.pgm"),
    ProgramCalls("boat.pgm"),
    ProgramCalls("bridge.pgm"),
    ProgramCalls("camera.pgm"),
    ProgramCalls("Fig.5.pgm"),
    ProgramCalls("Fig.17.pgm"),
    ProgramCalls("goldhill.pgm"),
    ProgramCalls("house.pgm"),
    ProgramCalls("lena.pgm"),
    ProgramCalls("mandrill.pgm"),
    ProgramCalls("mountain.pgm"),
    ProgramCalls("output6.pgm"),
    ProgramCalls("peppers.pgm"),
    ProgramCalls("w2.pgm"),
    ProgramCalls("w6.pgm"),
    ProgramCalls("washsat.pgm"),
    ProgramCalls("zelda.pgm")]

  runtime_incr_contour = []
  runtime_non_incr_contour = []
  nnodes_max_tree = []
  images = [] 
  width = []
  height = []
  npixels = []

  for pcall in RUNTIME_PROGRAM_CALLS:

    # Register input image
    images.append(pcall.image)

    # Register input image size
    f = read_image(pcall.image)

    # Register width 
    width.append(f.shape[1])

    # Register height
    height.append(f.shape[0])

    # Register number of pixels
    npixels.append(f.shape[0] * f.shape[1])

    # Register max-tree nodes
    nnodes_max_tree.append(max_tree_number_of_nodes(f))

    if START_INCREMENTAL:

      time.sleep(5)

      # Register incremental contour performace
      runtime_incr_contour.append(pcall.run_perf_incr_contour())

      time.sleep(5)

      # Register non-incremental contour performace
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour())
    
    else:
      time.sleep(5)

      # Register non-incremental contour performace
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour()) 

      time.sleep(5)

      # Register incremental contour performace
      runtime_incr_contour.append(pcall.run_perf_incr_contour())


  df = pd.DataFrame(data={
    "image": images,
    "nnodes": nnodes_max_tree,  
    "width": width,
    "height": height,
    "npixels": npixels,
    "runtime_incr_contour": runtime_incr_contour,
    "runtime_non_incr_contour": runtime_non_incr_contour})

  df.to_csv(OUTPUT_FILE_NAME, sep=";")
  print(F"runtime dataset has been written into '{OUTPUT_FILE_NAME}'")


# def main(OUTPUT_FILE_NAME= "runtime/runtime_9.csv",
#          IMAGE_BASE_PATH = "../programs/img",
#          START_INCREMENTAL = True):

parser = ArgumentParser(description="Generate runtime csv",
                        formatter_class=ArgumentDefaultsHelpFormatter)
parser.add_argument("-o", "--output-filename", 
                    help="output filename",
                    default="runtime/runtime_1.csv")
parser.add_argument("-i", "--image-base-path", 
                    help="base path where the input image are stored.",
                    default="../programs/img")
parser.add_argument("-s", "--start-incremental", 
                    help="should the program register incremental and \
                    non-incremental run-time or vice-versa.",
                    action="store_true")

args = parser.parse_args()
print(args)
main(OUTPUT_FILE_NAME=args.output_filename, 
     IMAGE_BASE_PATH=args.image_base_path,
     START_INCREMENTAL=args.start_incremental)
