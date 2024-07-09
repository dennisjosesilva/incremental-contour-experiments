from argparse import ArgumentParser, ArgumentDefaultsHelpFormatter


from imageio.v2 import imread, imwrite

from skimage.transform import resize, rescale 

from tqdm import tqdm
import numpy as np 


def main(IN_FOLDER="1920x1080/test",
        OUT_FOLDER="960x540/test",
        SEED=42,
        N_IMAGES=50,
        RESOLUTION=0.5):
  
  # Get image file paths
  def gather_image_filepaths():  
    import os 
    from os.path import isfile, join

    lfiles = [f for f in os.listdir(IN_FOLDER) 
              if isfile(join(IN_FOLDER, f))]
    return np.array(sorted(lfiles))
  
  # shufle image
  def shuffle(lfiles):   
    rgen = np.random.RandomState(SEED)
    idx = np.array(rgen.permutation(len(lfiles)))    
    return lfiles[idx] 
  
  # resize image 
  def save_resized_image(in_image):
    from os.path import join

    img = imread(join(IN_FOLDER, in_image))
    img = rescale(img, scale=RESOLUTION)
    img = (img * 255).astype(np.uint8)
    imwrite(join(OUT_FOLDER, in_image), img)


  lfiles = gather_image_filepaths()
  
  if SEED != None:
    lfiles = shuffle(lfiles)
  
  if N_IMAGES != None:
    lfiles = sorted(lfiles[:N_IMAGES])

  for f in tqdm(lfiles):
    import os.path    
    save_resized_image(f)

#################### CMD ARGUMENT ############################################################# 

parser = ArgumentParser(description="Scale batch images")

parser.add_argument("-i", "--in_folder", 
                    help="input folder with images",
                    default="1920x1080/test")
parser.add_argument("-o", "--out_folder",
                    help="Output folder where the scaled images will be saved",
                    default="960x540/test")
parser.add_argument("-s", "--seed", type=int,
                    help="Seed value for random number generetor (use None for system clock)",
                    default=None)
parser.add_argument("-n", "--n_images", type=int,
                    help="Set the number of images to be scaled down (use None for all in the input folder)",
                    default=None)
parser.add_argument("-r", "--resolution", type=float,
                    help="Set the resolution scale",
                    default=0.5)
args = parser.parse_args()
print(args)
print()

main(IN_FOLDER=args.in_folder,
     OUT_FOLDER=args.out_folder,
     SEED=args.seed,
     N_IMAGES=args.n_images,
     RESOLUTION=args.resolution)