#include <iostream>

#include <morphotree/adjacency/adjacency4c.hpp>
#include <morphotree/attributes/countorExtraction.hpp>

#include <morphotree/core/io.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <chrono>

namespace mt = morphotree;


void defaultImage();
void inputImage(const std::string &filepath);

int main(int argc, char *argv[])
{
  
  if (argc < 2) 
    defaultImage();
  else 
    inputImage(argv[1]);


}

void defaultImage()
{
  using mt::uint8;  
  using mt::uint32;
  using MTree = mt::MorphologicalTree<uint8>;
  using NodePtr = typename MTree::NodePtr;
  using mt::InfAdjacency4C;
  using mt::Adjacency4C;
  using mt::Adjacency;
  using mt::Box;
  using mt::extractCountors;
  using mt::reconstructContourImage;
  using mt::UI32Point;  
  using mt::buildMaxTree;
  using mt::printImageIntoConsoleWithCast;

  std::vector<uint8> f = {
    0, 0, 0, 0, 0, 0, 0,
    0, 4, 4, 4, 7, 7, 7,
    0, 7, 7, 4, 7, 4, 7,
    0, 7, 4, 4, 7, 4, 7,
    0, 4, 4, 4, 7, 4, 7,
    0, 7, 7, 4, 7, 7, 7,
    0, 0, 0, 0, 0, 0, 0
  };
  
  Box domain = Box::fromSize({ 7, 7});

  std::shared_ptr<Adjacency> adj = std::make_shared<Adjacency4C>(domain);
  MTree tree = buildMaxTree(f, adj);

  std::vector<std::unordered_set<uint32>> contours =
    extractCountors(domain, f, std::make_shared<InfAdjacency4C>(domain), tree);

  unsigned long sumContour = 0;
  unsigned long maxContour = 0;
  unsigned long sumChildren = 0;
  unsigned long maxChildren = 0;

  tree.tranverse([&](NodePtr node){
    sumChildren += node->children().size();
    sumContour += contours[node->id()].size();

    if (maxContour < contours[node->id()].size())
      maxContour = contours[node->id()].size();
    
    if (maxChildren < node->children().size())
      maxChildren = node->children().size();

    std::cout << "node.id= " << node->id() << "\n"
              << "nchidren= " << node->children().size() << "\n"
              << "ncontour= " << contours[node->id()].size() << "\n";

    std::vector<bool> contourImg = reconstructContourImage(
      contours[node->id()], domain);

    printImageIntoConsoleWithCast<int>(contourImg, domain);    
    std::cout << "\n";
  });  

  std::cout << "\n"
            << "nnodes= " << tree.numberOfNodes() << "\n"
            << "sum_children= " << sumChildren << "\n"
            << "max_children= " << maxChildren << "\n"
            << "sum_contour= " << sumContour << "\n"
            << "max_contour= " << maxContour << "\n"; 
}

void inputImage(const std::string &filepath)
{
  using mt::uint8;  
  using mt::uint32;
  using MTree = mt::MorphologicalTree<uint8>;
  using NodePtr = typename MTree::NodePtr;
  using mt::InfAdjacency4C;
  using mt::Adjacency4C;
  using mt::Adjacency;
  using mt::Box;
  using mt::extractCountors;  
  using mt::UI32Point;
  using mt::buildMaxTree;

  int nx, ny, nc;
  uint8 *data = stbi_load(filepath.c_str(), &nx, &ny, &nc, 0);

  std::vector<uint8> f(data, data + (nx* ny));
  Box domain = Box::fromSize({
    static_cast<uint32>(nx), static_cast<uint32>(ny)});

  std::shared_ptr<Adjacency> adj = std::make_shared<Adjacency4C>(domain);
  MTree tree = buildMaxTree(f, adj);

  std::vector<std::unordered_set<uint32>> contours =
    extractCountors(domain, f, std::make_shared<InfAdjacency4C>(domain), tree);

  unsigned long sumContour = 0;
  unsigned long maxContour = 0;
  unsigned long sumChildren = 0;
  unsigned long maxChildren = 0;

  tree.tranverse([&](NodePtr node){
    sumChildren += node->children().size();
    sumContour += contours[node->id()].size();

    if (maxContour < contours[node->id()].size())
      maxContour = contours[node->id()].size();
    
    if (maxChildren < node->children().size())
      maxChildren = node->children().size();
  });  

  std::cout << "nnodes= " << tree.numberOfNodes() << "\n"
            << "sum_children= " << sumChildren << "\n"
            << "max_children= " << maxChildren << "\n"
            << "sum_contour= " << sumContour << "\n"
            << "max_contour= " << maxContour << "\n"; 
}
