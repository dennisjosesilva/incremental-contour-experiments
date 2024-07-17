import subprocess
import re 

import time 
import pandas as pd 

import imageio.v2 as im 

from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter

def main( OUTPUT_FILE_NAME= "tree_data/tree_data_1920x1080.csv",
          IMAGE_BASE_PATH= "../dataset-icdar/1920x1080/test"):
  
  # =============================================================
  # Tree data structure
  # =============================================================
  class TData:
    def __init__(self, mo):
      self.nnodes_ = int(mo.group("nnodes"))
      self.sum_children_ = int(mo.group("sum_children"))
      self.max_children_ = int(mo.group("max_children"))
      self.sum_contour_ = int(mo.group("sum_contour"))
      self.max_contour_ = int(mo.group("max_contour"))

    @property
    def nnodes(self):
      return self.nnodes_
    
    @property
    def sum_children(self):
      return self.sum_children_

    @property
    def max_children(self):
      return self.max_children_
    
    @property
    def sum_contour(self):
      return self.sum_contour_
    
    @property
    def max_contour(self):
      return self.max_contour_


  # ===========================================================
  # Program calls 
  # ===========================================================
  class ProgramCalls:
    def __init__(self, image):
      self.image = image 

    @property
    def tdata_program(self):
      return f"../programs/build/mtree_data"
    
    def _run_program(self, program):
      full_program_call = f"{program} {IMAGE_BASE_PATH}/{self.image}"
      print(full_program_call)

      p = subprocess.Popen(full_program_call, stdout=subprocess.PIPE, shell=True)

      (output, err) = p.communicate()

      p_status = p.wait()
      p_output = output.decode()

      print(p_output)

      str_tdata_regex = \
        r"nnodes=\s+(?P<nnodes>\d+)\n"\
        "sum_children=\s+(?P<sum_children>\d+)\n"\
        "max_children=\s+(?P<max_children>\d+)\n"\
        "sum_contour=\s+(?P<sum_contour>\d+)\n"\
        "max_contour=\s+(?P<max_contour>\d+)\n"\
        

      print(p_output)
      tdata_regex = re.compile(str_tdata_regex)
      mo = tdata_regex.search(p_output)
      return TData(mo=mo)

    def run_tdata(self):
      return self._run_program(self.tdata_program)

  # ===========================================================
  # Extract program calls
  # ===========================================================  
  def extract_program_calls():
    import os
    from os.path import isfile, join 
    import numpy as np 

    lfiles = [f for f in os.listdir(IMAGE_BASE_PATH)
              if isfile(join(IMAGE_BASE_PATH, f))]
    
    return [ProgramCalls(f) for f in lfiles]

  # ==========================================================
  # Read image
  # ==========================================================
  def read_image(image):
    return im.imread(f"{IMAGE_BASE_PATH}/{image}")


  # ==========================================================
  # main 
  # ==========================================================
  PROGRAM_CALLS = extract_program_calls()

  images = []
  width = []
  height = []
  npixels = []
  nnodes = []
  sum_children = []
  max_children = []
  sum_contour = []
  max_contour = []

  for pcall in PROGRAM_CALLS:
    
    # Register input image 
    images.append(pcall.image)

    # Load input image 
    f = read_image(pcall.image)

    # Register width 
    width.append(f.shape[1])

    # Register height 
    height.append(f.shape[0])

    # Register number of pixels 
    npixels.append(f.shape[0] * f.shape[1])

    # Execute tdata program
    tdata = pcall.run_tdata()

    # Register nnodes
    nnodes.append(tdata.nnodes)

    # Register sum_children
    sum_children.append(tdata.sum_children)

    # Register max_children
    max_children.append(tdata.max_children)

    # Register sum_contour
    sum_contour.append(tdata.sum_contour)

    # Register max contour
    max_contour.append(tdata.max_contour)

  # Create DataFrame 
  df = pd.DataFrame(data={
    "image": images,    
    "width": width,
    "height": height,
    "npixels": npixels,    
    "nnodes": nnodes,
    "sum_children": sum_children,
    "max_children": max_children,
    "sum_contour": sum_contour,
    "max_contour": max_contour
  })

  df.to_csv(OUTPUT_FILE_NAME, sep=";")
  print(f"tree dataset has been written into '{OUTPUT_FILE_NAME}'")

# =================================================================================
# COMMAND LINE ARGUMENT 
# =================================================================================
parser = ArgumentParser(description="Log max-tree data", 
                        formatter_class=ArgumentDefaultsHelpFormatter)
parser.add_argument("-i", "--image-base-path",
                    help="Base path where the input images are stored",
                    default="../dataset-icdar/1920x1080/test")
parser.add_argument("-o", "--output-filename",
                    help="Output filename", 
                    default="tree_data/test50_1920x1080.csv")

args = parser.parse_args()
print(args)
main(OUTPUT_FILE_NAME=args.output_filename, IMAGE_BASE_PATH=args.image_base_path)