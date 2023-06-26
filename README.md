This repos

# Incremental component tree contour computation

Dennis J. Silva,  Ronaldo F. Hashimoto, Jiri Kosinka, Jos B.T.M. Roerdink, Wonder A.L. Alves, Alexandre Morimitsu

## Abstract

A component tree is a graph representation that encodes the connected components 
of the upper or lower level sets of a greyscale image. Consequently, the nodes 
of a component tree represent binary images of the encoded connected components. 
There exist various algorithms that extract information and attributes of nodes of a component tree. We propose an efficient incremental method to compute the contours of the nodes of a component  tree by counting the edges (sides) of contour pixels.

## Experiments

## 1. Building

The software needs a C++ compiler and [conan v2](https://conan.io/) to build.  It also depends on some a Conan packages that can be installed manually using the instruction bellow.

### 1.1. Manual dependency installation

To install a Conan package, we need to run a Conan command at the root of the repositories on the links down below. 

- [morphotree library](https://github.com/dennisjosesilva/morphotree) 

```bash
$ conan create . morphotree/0.0.1@dennisjosesilva/dev
```

### 1.2. Building programs

After the morphotree library is installed, we can compile the programs used to analyse our algorithm.  First, we should create a build directory where we will keep the binary files. To do so, using Linux, we can move to the path where the repository is download as follows:

```bash
cd path/to/repo/
```

Then, we create the build directory by running:

```bash
mkdir build
```

Now, everything is set up to install morphotree as a dependency of our programs. To install the dependencies using conan v2 we can run the following command:

```bash
conan install . --output-folder=build --building=missing
```

It will install the dependencies (morphotree) described in conanfile.txt, will generate the cmake setup files in the "build" directory and will building the source code if it does not find the binary at the conan local cache. 

The next step is to compile the programs using CMake. To to this we go the "build" directory and use CMake to generate the building files as follows:

```bash
cd build 
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
```

By doing so, CMake will generates the files that can be compiled by a C++ compile. We can, generally, compile the source code using the default CMake options running:

```bash
cmake --build .
```

Finally, everything is compiled and ready to run.  After this process, we should have the following programs and its respectively parameters in the build directory:

* **perf_incr_contour  <input_image_path>**: It runs and show the elapsed time of the incremental contour computation of the max-tree of the "input image" (read from <input_image_path>)  implemeted by morphotree library.

* **perf_incr_contour_hashmap <input_image_path>**: It runs and show the elapsed time of the incremental contour computation of the max-tree of the "input image" (read from <input_image_path>) implemeted using hash maps to store sets.

* **perf_incr_contour_red_black_tree <input_image_path>**: It runs and show the elapsed time of the incremental contour computation of the max-tree of the "input image" (read from <input_image_path>) implemeted using red black trees to store sets.

* **perf_non_incr_contour <input_image_path>**: It runs and show the elapsed time of the non-incremental contour computation of the max-tree of the "input image" (read from <input_image_path>).

* **perf_contour_trace <input_image_path>**: It runs an experimental idea of computing contour tracing.

* **check_contour_algorithms <input_image_path> <inTreeAdj= 4|8> <inContourAdj = 4|8>**: It runs the incremental contour computation implemeted in morphotree library (the same as the paper) and non-incremental algorithm and compares the results. If the results are not equal it prints the id of the node which they are different, otherwise it prints the results are OK. It performs the algorithms to the max-tree of the input image (read from <input_image_path>), <inTreeAdj> tree connectivy and <inContourAdj> connectivy of the contour.

* **end_slides_image**: It runs the incremental contour computation to compute the contour of a node for the tree of the image "goldhill.pgm" and saves the image "goldhill-highlight.png" highlighting the node contour computed in red. Application used to generate an image for presentation.

### 1.3. Running contour computation bash for all images in our dataset

Inside the "programs" directory, there is a bash which runs the algorithm checking program for all images in "img" (our dataset) for all possible parameters combinations. You can run this batch by running after building the programs (step 1.2):

```bash
bash perfom_check_dataset.sh
```

As the results show our incremental algorithm produces the same result as the non-incremental (brute-force, reconstruction-based approach) for all images in our dataset for all combination of adjacencies for the max-tree and contour.

TODO 


