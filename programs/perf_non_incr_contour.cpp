#include <iostream>

#include <morphotree/attributes/countorExtraction.hpp>
#include <morphotree/adjacency/adjacency4c.hpp>
#include <morphotree/tree/ct_builder.hpp>
#include <morphotree/attributes/boundingboxComputer.hpp>

#include <morphotree/core/io.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <chrono>

namespace mt = morphotree;





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
  using mt::extractCountors;
  using mt::reconstructContourImage;
  using mt::UI32Point;
  using mt::I32Point;
  using mt::buildMaxTree;
  using mt::printImageIntoConsoleWithCast;
  using mt::BoundingBoxComputer;

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
    
    // ----------------------------------------------------------------------------
    // NON-INCREMENTAL CONTOURS COMPUTATION
    // ----------------------------------------------------------------------------
    std::vector<std::unordered_set<uint32>> contours(tree.numberOfNodes());
    std::unique_ptr<Adjacency> cadj = std::make_unique<InfAdjacency4C>(domain);

    tree.traverseByLevel([&contours, &domain, &cadj, &f](NodePtr node){
      std::vector<uint32> bnode = node->reconstruct();
      std::unordered_set<uint32> Ncountor;
      for (uint32 pidx : bnode) {
        for (uint32 qidx : cadj->neighbours(pidx)) {
          I32Point q = domain.indexToPoint(qidx);
          if(!domain.contains(q) || f[qidx] < node->level()) {
            Ncountor.insert(pidx);
          }
        }
      }

      contours[node->id()] = Ncountor;
    });

    auto end = high_resolution_clock::now();

    auto timeElapsed = duration_cast<milliseconds>(end - start);
    std::cout << "time elapsed: " << timeElapsed.count() << "\n";

    // ------------------------------------------------------------------------------
    // RECONSTRUCT CONTOUR IMAGE FOR TESTING
    // ------------------------------------------------------------------------------
    // NodePtr node = tree.smallComponent(domain.pointToIndex(
    //   I32Point{nx/2, ny/2}));
    // const std::set<uint32> &contour = contours[node->id()];
    // std::vector<uint8> output(domain.numberOfPoints(), 255);
    // for (uint32 pidx : node->reconstruct()) 
    //   output[pidx] = 0;
    // for (uint32 pidx : contour)
    //   output[pidx] = 128;

    // stbi_write_png("out.png", nx, ny, 1, output.data(), 0);
  }
  else {
    std::cerr << "Error, it needs to receive a path for a image file";
  }

  return 0;
}