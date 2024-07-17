#!/usr/bin/python

import subprocess
import re 

import time
import pandas as pd 

import morphotreepy as mt
import imageio.v2 as im

from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter

def main(OUTPUT_FILE_NAME= "runtime-icdar-val/runtime_0.csv",
         IMAGE_BASE_PATH = "../dataset-icdar/validation",
         EXECUTATION_ORDER = "rhb"):

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
    def perf_incr_contour_hm(self):
      return f"../programs/build/perf_incr_contour_hashmap"

    @property
    def perf_incr_contour_rbt(self):
      return f"../programs/build/perf_incr_contour_red_black_tree"

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

    def run_perf_incr_contour_rbt(self):
      return self._run_program(self.perf_incr_contour_rbt)

    def run_perf_incr_contour_hm(self):
      return self._run_program(self.perf_incr_contour_hm)

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
  def extract_programs_calls():
    import os
    from os.path import isfile, join
    import numpy as np

    
    lfiles = [f for f in os.listdir(IMAGE_BASE_PATH) 
              if isfile(join(IMAGE_BASE_PATH, f))]    
        
    return [ProgramCalls(f) for f in lfiles]


  # ===================================================================
  # main
  # ===================================================================
  RUNTIME_PROGRAM_CALLS = extract_programs_calls()

  runtime_incr_contour_rbt = []
  runtime_incr_contour_hm = []
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

    if EXECUTATION_ORDER == "rhn":
      time.sleep(2)
      runtime_incr_contour_rbt.append(pcall.run_perf_incr_contour_rbt())
      time.sleep(2)
      runtime_incr_contour_hm.append(pcall.run_perf_incr_contour_hm())
      time.sleep(2)
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour())
    
    elif EXECUTATION_ORDER == "rnh":
      time.sleep(2)
      runtime_incr_contour_rbt.append(pcall.run_perf_incr_contour_rbt())
      time.sleep(2)
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour())
      time.sleep(2)
      runtime_incr_contour_hm.append(pcall.run_perf_incr_contour_hm())
    
    elif EXECUTATION_ORDER == "nhr":
      time.sleep(2)
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour())
      time.sleep(2)
      runtime_incr_contour_hm.append(pcall.run_perf_incr_contour_hm())
      time.sleep(2)
      runtime_incr_contour_rbt.append(pcall.run_perf_incr_contour_rbt())      
      
    elif EXECUTATION_ORDER == "nrh":
      time.sleep(2)
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour())
      time.sleep(2)
      runtime_incr_contour_rbt.append(pcall.run_perf_incr_contour_rbt())
      time.sleep(2)
      runtime_incr_contour_hm.append(pcall.run_perf_incr_contour_hm())
    
    elif EXECUTATION_ORDER == "hnr":
      time.sleep(2)
      runtime_incr_contour_hm.append(pcall.run_perf_incr_contour_hm())
      time.sleep(2)
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour())
      time.sleep(2)
      runtime_incr_contour_rbt.append(pcall.run_perf_incr_contour_rbt())
      
    elif EXECUTATION_ORDER == "hrn":
      time.sleep(2)
      runtime_incr_contour_hm.append(pcall.run_perf_incr_contour_hm())
      time.sleep(2)
      runtime_incr_contour_rbt.append(pcall.run_perf_incr_contour_rbt())
      time.sleep(2)
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour())
      
  df = pd.DataFrame(data={
    "image": images,
    "nnodes": nnodes_max_tree,  
    "width": width,
    "height": height,
    "npixels": npixels,
    "runtime_incr_contour_rb": runtime_incr_contour_rbt,
    "runtime_incr_contour_hm": runtime_incr_contour_hm,
    "runtime_non_incr_contour": runtime_non_incr_contour})

  df.to_csv(OUTPUT_FILE_NAME, sep=";")
  print(f"runtime dataset has been written into '{OUTPUT_FILE_NAME}'")


parser = ArgumentParser(description="Generate runtime csv",
                        formatter_class=ArgumentDefaultsHelpFormatter)
parser.add_argument("-o", "--output-filename", 
                    help="output filename",
                    default="runtime/runtime_checkboard_0.csv")
parser.add_argument("-i", "--image-base-path", 
                    help="base path where the input image are stored.",
                    default="../checkboard")
parser.add_argument("-e", "--execution-order", 
                    help="Order which the program run the different algorithms.",
                    default="rhn")

args = parser.parse_args()
print(args)
main(OUTPUT_FILE_NAME=args.output_filename, 
     IMAGE_BASE_PATH=args.image_base_path,
     EXECUTATION_ORDER=args.execution_order)