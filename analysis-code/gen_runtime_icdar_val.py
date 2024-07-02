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
  # main
  # ===================================================================
  RUNTIME_PROGRAM_CALLS = [
    ProgramCalls("val_000.png"),
    ProgramCalls("val_001.png"),
    ProgramCalls("val_002.png"),
    ProgramCalls("val_003.png"),
    ProgramCalls("val_004.png"),
    ProgramCalls("val_005.png"),
    ProgramCalls("val_006.png"),
    ProgramCalls("val_007.png"),
    ProgramCalls("val_008.png"),
    ProgramCalls("val_009.png"),
    ProgramCalls("val_010.png"),
    ProgramCalls("val_011.png"),
    ProgramCalls("val_012.png"),
    ProgramCalls("val_013.png"),
    ProgramCalls("val_014.png"),
    ProgramCalls("val_015.png"),
    ProgramCalls("val_016.png"),
    ProgramCalls("val_017.png"),
    ProgramCalls("val_018.png"),
    ProgramCalls("val_019.png"),
    ProgramCalls("val_020.png"),
    ProgramCalls("val_021.png"),
    ProgramCalls("val_022.png"),
    ProgramCalls("val_023.png"),
    ProgramCalls("val_024.png"),
    ProgramCalls("val_025.png"),
    ProgramCalls("val_026.png"),
    ProgramCalls("val_027.png"),
    ProgramCalls("val_028.png"),
    ProgramCalls("val_029.png"),
    ProgramCalls("val_030.png"),
    ProgramCalls("val_031.png"),
    ProgramCalls("val_032.png"),
    ProgramCalls("val_033.png"),
    ProgramCalls("val_034.png"),
    ProgramCalls("val_035.png"),
    ProgramCalls("val_036.png"),
    ProgramCalls("val_037.png"),
    ProgramCalls("val_038.png"),
    ProgramCalls("val_039.png"),
    ProgramCalls("val_040.png"),
    ProgramCalls("val_041.png"),
    ProgramCalls("val_042.png"),
    ProgramCalls("val_043.png"),
    ProgramCalls("val_044.png"),
    ProgramCalls("val_045.png"),
    ProgramCalls("val_046.png"),
    ProgramCalls("val_047.png"),
    ProgramCalls("val_048.png"),
    ProgramCalls("val_049.png"),
    ProgramCalls("val_050.png"),
    ProgramCalls("val_051.png"),
    ProgramCalls("val_052.png"),
    ProgramCalls("val_053.png"),
    ProgramCalls("val_054.png"),
    ProgramCalls("val_055.png"),
    ProgramCalls("val_056.png"),
    ProgramCalls("val_057.png"),
    ProgramCalls("val_058.png"),
    ProgramCalls("val_059.png"),
    ProgramCalls("val_060.png"),
    ProgramCalls("val_061.png"),
    ProgramCalls("val_062.png"),
    ProgramCalls("val_063.png"),
    ProgramCalls("val_064.png"),
    ProgramCalls("val_065.png"),
    ProgramCalls("val_066.png"),
    ProgramCalls("val_067.png"),
    ProgramCalls("val_068.png"),
    ProgramCalls("val_069.png"),
    ProgramCalls("val_070.png"),
    ProgramCalls("val_071.png"),
    ProgramCalls("val_072.png"),
    ProgramCalls("val_073.png"),
    ProgramCalls("val_074.png"),
    ProgramCalls("val_075.png"),
    ProgramCalls("val_076.png"),
    ProgramCalls("val_077.png"),
    ProgramCalls("val_078.png"),
    ProgramCalls("val_079.png"),
    ProgramCalls("val_080.png"),
    ProgramCalls("val_081.png"),
    ProgramCalls("val_082.png"),
    ProgramCalls("val_083.png"),
    ProgramCalls("val_084.png"),
    ProgramCalls("val_085.png"),
    ProgramCalls("val_086.png"),
    ProgramCalls("val_087.png"),
    ProgramCalls("val_088.png"),
    ProgramCalls("val_089.png"),
    ProgramCalls("val_090.png"),
    ProgramCalls("val_091.png"),
    ProgramCalls("val_092.png"),
    ProgramCalls("val_093.png"),
    ProgramCalls("val_094.png"),
    ProgramCalls("val_095.png"),
    ProgramCalls("val_096.png"),
    ProgramCalls("val_097.png"),
    ProgramCalls("val_098.png"),
    ProgramCalls("val_099.png"),
    ProgramCalls("val_100.png"),
    ProgramCalls("val_101.png"),
    ProgramCalls("val_102.png"),
    ProgramCalls("val_103.png"),
    ProgramCalls("val_104.png"),
    ProgramCalls("val_105.png"),
    ProgramCalls("val_106.png"),
    ProgramCalls("val_107.png"),
    ProgramCalls("val_108.png"),
    ProgramCalls("val_109.png"),
    ProgramCalls("val_110.png"),
    ProgramCalls("val_111.png"),
    ProgramCalls("val_112.png"),
    ProgramCalls("val_113.png"),
    ProgramCalls("val_114.png"),
    ProgramCalls("val_115.png"),
    ProgramCalls("val_116.png"),
    ProgramCalls("val_117.png"),
    ProgramCalls("val_118.png"),
    ProgramCalls("val_119.png"),
    ProgramCalls("val_120.png"),
    ProgramCalls("val_121.png"),
    ProgramCalls("val_122.png"),
    ProgramCalls("val_123.png"),
    ProgramCalls("val_124.png"),
    ProgramCalls("val_125.png"),
    ProgramCalls("val_126.png"),
    ProgramCalls("val_127.png"),
    ProgramCalls("val_128.png"),
    ProgramCalls("val_129.png"),
    ProgramCalls("val_130.png"),
    ProgramCalls("val_131.png"),
    ProgramCalls("val_132.png"),
    ProgramCalls("val_133.png"),
    ProgramCalls("val_134.png"),
    ProgramCalls("val_135.png"),
    ProgramCalls("val_136.png"),
    ProgramCalls("val_137.png"),
    ProgramCalls("val_138.png"),
    ProgramCalls("val_139.png"),
    ProgramCalls("val_140.png"),
    ProgramCalls("val_141.png"),
    ProgramCalls("val_142.png"),
    ProgramCalls("val_143.png"),
    ProgramCalls("val_144.png"),
    ProgramCalls("val_145.png"),
    ProgramCalls("val_146.png"),
    ProgramCalls("val_147.png"),
    ProgramCalls("val_148.png"),
    ProgramCalls("val_149.png"),
    ProgramCalls("val_150.png"),
    ProgramCalls("val_151.png"),
    ProgramCalls("val_152.png"),
    ProgramCalls("val_153.png"),
    ProgramCalls("val_154.png"),
    ProgramCalls("val_155.png"),
    ProgramCalls("val_156.png"),
    ProgramCalls("val_157.png"),
    ProgramCalls("val_158.png"),
    ProgramCalls("val_159.png"),
    ProgramCalls("val_160.png"),
    ProgramCalls("val_161.png"),
    ProgramCalls("val_162.png"),
    ProgramCalls("val_163.png"),
    ProgramCalls("val_164.png"),
    ProgramCalls("val_165.png"),
    ProgramCalls("val_166.png"),
    ProgramCalls("val_167.png"),
    ProgramCalls("val_168.png"),
    ProgramCalls("val_169.png"),
    ProgramCalls("val_170.png"),
    ProgramCalls("val_171.png"),
    ProgramCalls("val_172.png"),
    ProgramCalls("val_173.png"),
    ProgramCalls("val_174.png"),
    ProgramCalls("val_175.png"),
    ProgramCalls("val_176.png"),
    ProgramCalls("val_177.png"),
    ProgramCalls("val_178.png"),
    ProgramCalls("val_179.png"),
    ProgramCalls("val_180.png"),
    ProgramCalls("val_181.png"),
    ProgramCalls("val_182.png"),
    ProgramCalls("val_183.png"),
    ProgramCalls("val_184.png"),
    ProgramCalls("val_185.png"),
    ProgramCalls("val_186.png"),
    ProgramCalls("val_187.png"),
    ProgramCalls("val_188.png"),
    ProgramCalls("val_189.png"),
    ProgramCalls("val_190.png"),
    ProgramCalls("val_191.png"),
    ProgramCalls("val_192.png"),
    ProgramCalls("val_193.png"),
    ProgramCalls("val_194.png"),
    ProgramCalls("val_195.png"),
    ProgramCalls("val_196.png"),
    ProgramCalls("val_197.png"),
    ProgramCalls("val_198.png"),
    ProgramCalls("val_199.png"),
    ProgramCalls("val_200.png"),
    ProgramCalls("val_201.png")]

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
      time.sleep(5)
      runtime_incr_contour_rbt.append(pcall.run_perf_incr_contour_rbt())
      time.sleep(5)
      runtime_incr_contour_hm.append(pcall.run_perf_incr_contour_hm())
      time.sleep(5)
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour())
    
    elif EXECUTATION_ORDER == "rnh":
      time.sleep(5)
      runtime_incr_contour_rbt.append(pcall.run_perf_incr_contour_rbt())
      time.sleep(5)
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour())
      time.sleep(5)
      runtime_incr_contour_hm.append(pcall.run_perf_incr_contour_hm())
    
    elif EXECUTATION_ORDER == "nhr":
      time.sleep(5)
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour())
      time.sleep(5)
      runtime_incr_contour_hm.append(pcall.run_perf_incr_contour_hm())
      time.sleep(5)
      runtime_incr_contour_rbt.append(pcall.run_perf_incr_contour_rbt())      
      
    elif EXECUTATION_ORDER == "nrh":
      time.sleep(5)
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour())
      time.sleep(5)
      runtime_incr_contour_rbt.append(pcall.run_perf_incr_contour_rbt())
      time.sleep(5)
      runtime_incr_contour_hm.append(pcall.run_perf_incr_contour_hm())
    
    elif EXECUTATION_ORDER == "hnr":
      time.sleep(5)
      runtime_incr_contour_hm.append(pcall.run_perf_incr_contour_hm())
      time.sleep(5)
      runtime_non_incr_contour.append(pcall.run_perf_non_incr_contour())
      time.sleep(5)
      runtime_incr_contour_rbt.append(pcall.run_perf_incr_contour_rbt())
      
    elif EXECUTATION_ORDER == "hrn":
      time.sleep(5)
      runtime_incr_contour_hm.append(pcall.run_perf_incr_contour_hm())
      time.sleep(5)
      runtime_incr_contour_rbt.append(pcall.run_perf_incr_contour_rbt())
      time.sleep(5)
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
