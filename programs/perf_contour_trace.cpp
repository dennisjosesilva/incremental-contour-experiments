#include <morphotree/core/box.hpp>
#include <morphotree/core/alias.hpp>

#include <iostream>
#include <vector>

#include <morphotree/attributes/countorExtraction.hpp>
#include <morphotree/adjacency/adjacency4c.hpp>
#include <morphotree/adjacency/adjacency8c.hpp>
#include <morphotree/tree/ct_builder.hpp>
#include <morphotree/attributes/boundingboxComputer.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <chrono>


namespace mt = morphotree;

struct TracedContour
{
  using Contour = std::vector<mt::uint32>;

  std::vector<Contour> Couter;
  std::vector<Contour> Cinner;
};

class ContourTracer
{
public:
  using uint8 = mt::uint8;
  using uint32 = mt::uint32;
  using Box = mt::Box;
  using I32Point = mt::I32Point;
  using Contour = TracedContour::Contour;

  using PtDir = std::pair<I32Point, int>;

  ContourTracer(const Box &domain, const std::vector<uint8> &f);

  TracedContour computeContour(const Box &bdomain, uint8 nodeLevel);
  Contour traceContour(uint8 nodeLevel, const I32Point &xs, int dS, int label);
  PtDir findNextPoint(uint8 nodeLevel, const I32Point &xc, int d);

  bool bval(uint8 bval, const I32Point &p) const;

private:
  static const std::vector<I32Point> DELTA;  

private:  
  const std::vector<uint8> &f_;
  std::vector<int> L_;
  Box Ldomain_;
  Box domain_;
};


// ==========================================================================================
//  MAIN 
// ==========================================================================================
int main(int argc, char *argv[])
{
  using mt::uint8;
  using mt::uint32;
  using MTree = mt::MorphologicalTree<uint8>;
  using NodePtr = typename MTree::NodePtr;
  using mt::InfAdjacency4C;
  using mt::Adjacency4C;
  using mt::Adjacency8C;
  using mt::Adjacency;
  using mt::Box;
  using mt::UI32Point;
  using mt::I32Point;
  using mt::buildMaxTree;
  using mt::BoundingBoxComputer;

  using std::chrono::high_resolution_clock;
  using std::chrono::duration;
  using std::chrono::duration_cast;
  using std::chrono::milliseconds;

  using timepoint = decltype(high_resolution_clock::now());

  if (argc > 1) {
    int nx, ny, nc;
    uint8 *data = stbi_load(argv[1], &nx, &ny, &nc, 0);

    std::vector<uint8> f(data, data + (nx * ny));
    Box domain = Box::fromSize(
      UI32Point{static_cast<uint32>(nx), static_cast<uint32>(ny)});
    
    std::shared_ptr<Adjacency> adj = std::make_shared<Adjacency8C>(domain);
    MTree tree = buildMaxTree(f, adj);

    std::vector<Box> bb = 
      std::make_unique<BoundingBoxComputer<uint8>>(domain)->computeAttribute(tree);

    timepoint start = high_resolution_clock::now();

    // ========================================================================================
    // CONTOUR TRACE 
    // ========================================================================================
    std::vector<TracedContour> contours(tree.numberOfNodes());
    ContourTracer ct{domain, f};

    tree.traverseByLevel([&contours, &domain, &ct, &bb](NodePtr node){
      contours[node->id()] = ct.computeContour(bb[node->id()], node->level());
    });

    timepoint end = high_resolution_clock::now();
    milliseconds timeElapsed = duration_cast<milliseconds>(end - start);
    std::cout << "time elapsed: " << timeElapsed.count() << "\n";

    // ===========================================================================================
    // RECONSTRUCT IMAGE FOR TESTING
    // ===========================================================================================
    NodePtr node = tree.smallComponent(domain.pointToIndex(I32Point(nx/2, ny/2)));
    const TracedContour &contour = contours[node->id()];
    std::vector<uint8> output(domain.numberOfPoints(), 255);
    
    for (uint32 pidx : node->reconstruct())
      output[pidx] = 0;
    
    for (TracedContour::Contour c : contour.Cinner) {
      for (uint32 pidx : c) 
        output[pidx] = 128;
    }

    for (TracedContour::Contour c : contour.Couter) {
      for (uint32 pidx : c) 
        output[pidx] = 128;
    }

    stbi_write_png("out.png", nx, ny, 1, output.data(), 0);
  }

  return 0;
}

// ==========================================================================================
// IMPLEMENTATION OF CONTOUR TRACER
// ==========================================================================================
const std::vector<morphotree::I32Point> ContourTracer::DELTA = {
    I32Point{ 1, 0}, I32Point{ 1, 1}, I32Point{ 0, 1}, I32Point{-1, 1},
    I32Point{-1, 0}, I32Point{-1,-1}, I32Point{ 0,-1}, I32Point{ 1,-1}};
  
ContourTracer::ContourTracer(const Box &domain, const std::vector<uint8> &f)
  :domain_{domain}, f_{f}
{
  Ldomain_ = Box::fromCorners(
    domain.topleft() + I32Point{-1,-1},
    domain.bottomright() + I32Point{1,1});
  L_.resize(Ldomain_.numberOfPoints());
}

TracedContour ContourTracer::computeContour(const Box &bdomain, uint8 nodeLevel)
{
  using Contour = TracedContour::Contour;

  TracedContour tc;   // create two empty sets of contours
  std::fill(L_.begin(), L_.end(), 0); // create a label map L
  int R = 0;  // Region counter R

  // Scan the image from left to right and top to bottom
  I32Point p;
  for (p.y() = bdomain.top(); p.y() <= bdomain.bottom(); p.y()++) {
    int l = 0;   // set the current label l to "none"
    for (p.x() = bdomain.left(); p.x() <= bdomain.right(); p.x()++) {
      if (bval(nodeLevel, p)) {
        if (l != 0)  // continue inside region
          L_[Ldomain_.pointToIndex(p)] = l; 
        else {
          l = L_[Ldomain_.pointToIndex(p)];
          if (l == 0) {   // hit a new outer contour
            R++;
            l = R;
            Contour c = traceContour(nodeLevel, p, 0, l);
            tc.Couter.push_back(c);      // collect outer contour
            L_[Ldomain_.pointToIndex(p)] = l;
          }
        }
      }
      else {    // background pixel
        if (l != 0) {
          if (L_[Ldomain_.pointToIndex(p)] == 0) {  // hit new inner contour
            I32Point xS = p + I32Point{-1, 0};
            Contour c = traceContour(nodeLevel, xS, 1, l);
            tc.Cinner.push_back(c);           // collect inner contour
          }
          l = 0;
        }
      }        
    }
  }
  return tc;
}

// Contour traceContour(const I32Point &xs, int dS, int label);
// uint32 findNextPoint(const I32Point &xc, int d);

ContourTracer::Contour ContourTracer::traceContour(uint8 nodeLevel,
  const I32Point &xs, int dS, int label) 
{
  PtDir pd = findNextPoint(nodeLevel, xs, dS);
  I32Point xt = pd.first;
  int dnext = pd.second;

  Contour c{domain_.pointToIndex(xt)};   // create a contour starting with xT
  I32Point xp = xs;                      // previous position xp
  I32Point xc = xt;                      // current position xc

  bool done = xs == xt;                  // isolated pixel?

  while (!done) {
     L_[Ldomain_.pointToIndex(xc)] = label;
    int dsearch = (dnext + 6) % 8;
    PtDir xn_dnext = findNextPoint(nodeLevel, xc, dsearch);
    I32Point xn = xn_dnext.first;
    dnext = xn_dnext.second;
    
    xp = xc;
    xc = xn;
    done = (xp == xs && xc == xt);   // back at start point?
    
    if (!done)
      c.push_back(domain_.pointToIndex(xn));
  }
  return c;
}

ContourTracer::PtDir ContourTracer::findNextPoint(uint8 nodeLevel,
  const I32Point &xc, int d)
{
  PtDir pd;
  for (int i = 0; i <= 6; i++) {
    I32Point xprime = xc + DELTA[d];
    if (!bval(nodeLevel, xprime)) {
      L_[Ldomain_.pointToIndex(xprime)] = -1;
      d = (d + 1) % 8;
    }
    else 
      return std::make_pair(xprime, d);
  }
  return std::make_pair(xc, d);
}

bool ContourTracer::bval(uint8 bval, const I32Point &p) const 
{
  if (domain_.contains(p))
    return f_[domain_.pointToIndex(p)] >= bval;
  
  // p outside domain correspond to background pixel
  return false;
}