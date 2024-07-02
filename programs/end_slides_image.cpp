#include "contour/nonincremental.hpp"

#include <morphotree/core/box.hpp>
#include <morphotree/attributes/countorExtraction.hpp>

#include <morphotree/adjacency/adjacency4c.hpp>
#include <morphotree/adjacency/adjacency8c.hpp>
#include <morphotree/attributes/areaComputer.hpp>

#include <morphotree/core/io.hpp>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

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
  using morphotree::I32Point;
  using morphotree::AreaComputer;

  using morphotree::extractCountors;
  using morphotree::reconstructContourImage;

   // =================================================
  // Define Variables 
  // =================================================
  std::vector<uint8> f;
  Box domain;
  std::shared_ptr<Adjacency> treeAdj;
  std::shared_ptr<Adjacency> contourAdj;

  int width, height, nchannels;
  uint8 *data = stbi_load("../img/goldhill.pgm", &width, &height, &nchannels, 1);
  
  domain = Box::fromSize({static_cast<uint32>(width), static_cast<uint32>(height)});
  f = std::vector<uint8>(data, data + domain.numberOfPoints());


  std::vector<uint8> outputContour(domain.numberOfPoints()*3);
  std::vector<uint8> outputNode(domain.numberOfPoints()*3);
  I32Point p;
  for (p.y() = domain.top(); p.y() <= domain.bottom(); p.y()++) {
    for (p.x() = domain.left(); p.x() <= domain.right(); p.x()++) {
      uint32 pidx = domain.pointToIndex(p); 
      uint32 cpidx = pidx * 3;           // (coloured) for 3 channels
      
      outputContour[cpidx] = f[pidx];
      outputContour[cpidx+1] = f[pidx];
      outputContour[cpidx+2] = f[pidx];

      outputNode[cpidx] = f[pidx];
      outputNode[cpidx+1] = f[pidx];
      outputNode[cpidx+2] = f[pidx];
    }
  }

  MTree tree = buildMaxTree(f, std::make_unique<Adjacency8C>(domain));  

  std::vector<uint32> area = 
    std::make_unique<AreaComputer<uint8>>()->computeAttribute(tree);
  tree.idirectFilter([&area](NodePtr node) { return area[node->id()] > 6100;} );
  std::vector<std::unordered_set<uint32>> contours = 
    extractCountors(domain, f, std::make_shared<InfAdjacency4C>(domain), tree);

  NodePtr node = tree.smallComponent(domain.pointToIndex({226, 341}));

  const std::unordered_set<uint32> &c = contours[node->id()];
  for (const uint32 pidx : c) {
    uint32 cpidx = pidx * 3;           // (coloured) for 3 channels
    
    outputContour[cpidx] =  255;
    outputContour[cpidx+1] = 0;
    outputContour[cpidx+2] = 0;
  }


  std::vector<uint32> nodePixels = node->reconstruct();    
  for (const uint32 pidx : nodePixels) {
    uint32 cpidx = pidx * 3;           // (coloured) for 3 channels
    
    outputNode[cpidx] =  255;
    outputNode[cpidx+1] = 0;
    outputNode[cpidx+2] = 0;
  }

  stbi_write_png("goldhill-highlight-contour.png", width, height, 3, outputContour.data(), 0);
  stbi_write_png("goldhill-highlight-node.png", width, height, 3, outputNode.data(), 0);
  stbi_image_free(data);
  return 0;
}