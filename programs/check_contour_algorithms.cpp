#include "contour/nonincremental.hpp"

#include <morphotree/core/box.hpp>
#include <morphotree/attributes/countorExtraction.hpp>

#include <morphotree/adjacency/adjacency4c.hpp>
#include <morphotree/adjacency/adjacency8c.hpp>

#include <morphotree/core/io.hpp>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


bool isEqual(const morphotree::Box &domain, const std::vector<bool> &a, 
  const std::vector<bool> &b)
{
  using morphotree::uint32;

  for (uint32 pidx = 0; pidx < domain.numberOfPoints(); pidx++) {
    if (a[pidx] != b[pidx])
      return false;
  }

  return true;
}

int main(int argc, char *argv[]) 
{
  using morphotree::uint8;
  using morphotree::uint32;
  using morphotree::Adjacency;
  using morphotree::InfAdjacency4C;
  using morphotree::InfAdjacency8C;
  using morphotree::Adjacency4C;
  using morphotree::Adjacency8C;
  using morphotree::Box;
  using MTree = morphotree::MorphologicalTree<uint8>;
  using morphotree::buildMaxTree;
  using NodePtr = typename MTree::NodePtr;
  using morphotree::UI32Point;

  using morphotree::extractCountors;
  using morphotree::reconstructContourImage;

  // =================================================
  // Define Variables 
  // =================================================
  std::vector<uint8> f;
  Box domain;
  std::shared_ptr<Adjacency> treeAdj;
  std::shared_ptr<Adjacency> contourAdj;
  char inTreeAdj;
  char inContourAdj;

  // =================================================
  // LOAD IMAGE 
  // =================================================  
  if (argc < 3) {
    // too few parameters
    std::cerr << "ERROR: CMD PARAMS" << std::endl;
    return -1; 
  }
  else if (argc < 4) {
    // default image
    // f = {
    //   0, 0, 0, 0, 0, 0, 0,
    //   0, 4, 4, 4, 7, 7, 7,
    //   0, 7, 7, 4 ,7, 4, 7,
    //   0, 7, 4, 4, 7, 4, 7,
    //   0, 4, 4, 4, 7, 4, 7,
    //   0, 7, 7, 4, 7, 7, 7,
    //   0, 0, 0, 0, 0, 0, 0
    // };
    f = {
      0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 1, 1, 0, 0,
      0, 1, 1, 1, 1, 1, 0,
      0, 1, 1, 1, 1, 1, 0,
      0, 0, 1, 1, 1, 0, 0,
      0, 0, 0, 0, 0, 0, 0
    };

    domain = Box::fromSize(UI32Point{7, 7});

    inTreeAdj = argv[1][0];
    inContourAdj = argv[2][0];
  }
  else {
    // suppose correct cmd params
    int width, height, nchannels;
    uint8 *data = stbi_load(argv[1], &width, &height, &nchannels, 1);
    
    domain = Box::fromSize({static_cast<uint32>(width), static_cast<uint32>(height)});
    f = std::vector<uint8>(data, data + domain.numberOfPoints());

    inTreeAdj = argv[2][0];
    inContourAdj = argv[3][0];
  }

  // ========================================================
  // DEFINE ADJACENCY RELATIONS
  // ======================================================== 
  if (inTreeAdj == '4')
    treeAdj = std::make_shared<Adjacency4C>(domain);
  else if (inTreeAdj == '8')  
    treeAdj = std::make_shared<Adjacency8C>(domain);

  if (inContourAdj == '4')
    contourAdj = std::make_shared<InfAdjacency4C>(domain);
  else if (inContourAdj == '8')
    contourAdj = std::make_shared<InfAdjacency8C>(domain);


  // ===========================================================
  // BUILD MAX-TREE 
  // ===========================================================
  MTree tree = buildMaxTree(f, treeAdj);

  // ==========================================================
  // INCREMENTAL COMPUTATION OF CONTOURS
  // ==========================================================
  std::vector<std::unordered_set<uint32>> incrContours = 
    extractCountors(domain, f, contourAdj, tree);

  // ==========================================================
  // NON INCREMENTAL COMPUTATION OF CONTOURS
  // ==========================================================
  std::vector<std::vector<bool>> nonIncrContours(tree.numberOfNodes());
  
  tree.tranverse([&domain, &nonIncrContours, contourAdj](NodePtr node){
    std::vector<bool> nimg = node->reconstruct(domain);
    nonIncrContours[node->id()] = 
      computeContourNonIncremental(contourAdj, domain, nimg);  
    //std::cout << node->id() << " contour computed\n";  
  });

  bool hasDifferentNode = false;
  tree.tranverse([&domain, &nonIncrContours, &incrContours, &hasDifferentNode](NodePtr node) {
    const std::vector<bool> &nonIncrContourImg = nonIncrContours[node->id()];
    std::vector<bool> incrContourImg = 
      reconstructContourImage(incrContours[node->id()], domain);

    if (!isEqual(domain, nonIncrContourImg, incrContourImg)) {
      std::cout << "Contours for node " << node->id() << " are NOT equal!\n";
      hasDifferentNode = true;
    }

    // if (isEqual(domain, nonIncrContourImg, incrContourImg)) {
    //   std::cout << "contours for node " << node->id() << " are equal!\n";      
    //   morphotree::printImageIntoConsoleWithCast<int>(node->reconstruct(domain), domain);
    //   std::cout << "\n";
    //   morphotree::printImageIntoConsoleWithCast<int>(nonIncrContourImg, domain);
    //   std::cout << "\n";
    //   morphotree::printImageIntoConsoleWithCast<int>(incrContourImg, domain);
    // }
    // else {
    //   std::cout << "contours for node " << node->id() << " are NOT equal!\n";
    //   // morphotree::printImageIntoConsoleWithCast<int>(nonIncrContourImg, domain);
    //   // std::cout << "\n";
    //   // morphotree::printImageIntoConsoleWithCast<int>(incrContourImg, domain);
    // }
  });

  if (hasDifferentNode) 
    std::cout << "The program found an error on incremental algorithm";
  else
    std::cout << "The program is finished and not error was found";

  if (argc >= 4)
    std::cout << " for " << argv[1] << " and tree-" << inTreeAdj << " contour-" << inContourAdj << ".\n";
  else
    std::cout << ".\n";

  return 0;
}