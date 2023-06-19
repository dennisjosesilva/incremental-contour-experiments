#include <iostream>

#include <morphotree/adjacency/adjacency4c.hpp>
#include <morphotree/tree/ct_builder.hpp>

#include <morphotree/core/io.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <chrono>


#include <morphotree/adjacency/adjacency.hpp>
#include <morphotree/core/box.hpp>
#include <morphotree/tree/mtree.hpp>

#include <unordered_set>

namespace mt = morphotree;

template<class ValueType>
std::vector<std::unordered_set<mt::uint32>> extractCountorsHashMap(
  const mt::Box &domain,
  const std::vector<ValueType> &f,
  std::shared_ptr<mt::Adjacency> adj,
  const mt::MorphologicalTree<ValueType> &tree);

std::vector<bool> reconstructContourImage(const std::unordered_set<mt::uint32>& contour, 
  const mt::Box &domain);

// =================== [IMPLEMENTATION] ==========================
template<class ValueType>
std::vector<std::unordered_set<mt::uint32>> extractCountorsHashMap(
  const mt::Box &domain, 
  const std::vector<ValueType> &f,
  std::shared_ptr<mt::Adjacency> adj,
  const mt::MorphologicalTree<ValueType> &tree)
{
  using MTree = mt::MorphologicalTree<ValueType>;
  using mt::uint32;
  using mt::uint8;
  using mt::Box;
  using NodePtr = typename MTree::NodePtr;

  std::vector<std::unordered_set<uint32>> contours(tree.numberOfNodes());
  std::vector<uint8> ncount(domain.numberOfPoints());

  tree.tranverse([&f, &contours, &ncount, adj](NodePtr node){
    
    // Initialise contours of node "N"
    std::unordered_set<uint32> &Ncontour = contours[node->id()];
    for (NodePtr c : node->children()) {
      for (uint32 pidx : contours[c->id()])
        Ncontour.insert(pidx);
    }

    for (uint32 pidx : node->cnps()) {
      for (uint32 qidx : adj->neighbours(pidx)) {
        if (qidx == Box::UndefinedIndex || f[pidx] > f[qidx]) 
          ncount[pidx]++;
        else if (f[pidx] < f[qidx]) {
          ncount[qidx]--;

          if (ncount[qidx] == 0) 
            Ncontour.erase(qidx);
        }
      }

      if (ncount[pidx] > 0)
        Ncontour.insert(pidx);
    }
  });
  
  return contours;
}


std::vector<bool> reconstructContourImage(const std::unordered_set<mt::uint32>& contour, 
    const mt::Box &domain)
{
  std::vector<bool> bimg(domain.numberOfPoints(), false);
  for (mt::uint32 pidx : contour) {
    bimg[pidx] = true;
  }

  return bimg;
}

int main(int argc, char *argv[]) 
{
  using mt::uint8;
  using mt::uint32;
  using MTree = mt::MorphologicalTree<uint8>;
  using NodePtr = typename MTree::NodePtr;
  using mt::InfAdjacency4C;
  using mt::Adjacency4C;
  using mt::Adjacency;
  using mt::Box;
  using mt::UI32Point;
  using mt::I32Point;
  using mt::buildMaxTree;
  using mt::printImageIntoConsoleWithCast;

  using std::chrono::high_resolution_clock;  
  using std::chrono::duration_cast;
  using std::chrono::duration;
  using std::chrono::milliseconds;
  
  if (argc > 1) {
    int nx, ny, nc;
    uint8 *data = stbi_load(argv[1], &nx, &ny, &nc, 0);

    std::vector<uint8> f(data, data + (nx*ny));
    Box domain = Box::fromSize(UI32Point{
      static_cast<uint32>(nx), static_cast<uint32>(ny)});

    std::shared_ptr<Adjacency> adj = std::make_shared<Adjacency4C>(domain);
    MTree tree = buildMaxTree(f, adj);

    auto start = high_resolution_clock::now();
    std::vector<std::unordered_set<uint32>> contours = 
      extractCountorsHashMap(domain, f, std::make_shared<InfAdjacency4C>(domain),
      tree);
    auto end = high_resolution_clock::now();

    auto timeElapsed = duration_cast<milliseconds>(end - start);
    std::cout << "time elapsed: " << timeElapsed.count() << "\n";
  }
  else {
    std::cerr << "Error, it needs to receive a path for a image file";
  }

  return 0;
}