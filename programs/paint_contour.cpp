#include <morphotree/core/box.hpp>
#include <morphotree/attributes/countorExtraction.hpp>

#include <morphotree/adjacency/adjacency4c.hpp>
#include <morphotree/adjacency/adjacency8c.hpp>
#include <morphotree/attributes/areaComputer.hpp>

#include <morphotree/core/io.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

static const int SEED = 42;
static const float SATURATION = 0.8f;
static const float VALUE = 0.9f;

struct Args
{
  std::string inImage;
  std::string outImage;
  int ncontours;
  morphotree::uint32 area;
};


Args extractCommandArgs(int argc, char *argv[])
{
  Args args;

  int i = 1;
  while (i < argc) {
    std::string option{ argv[i] };
    i++;
    
    if (option == "-i") {
      args.inImage = std::string{argv[i]};
    }
    else if (option == "-o") {
      args.outImage = std::string{argv[i]};
    }
    else if (option == "-n") {
      args.ncontours = std::atoi(argv[i]);
    }
    else if (option == "-a") {
      args.area = std::atoi(argv[i]);
    }
    i++;
  }

  return args;
}

struct Color {
  morphotree::uint8 r, g, b;
};

Color HSVtoRGB(float h, float s, float v)
{
  using morphotree::uint8;

  float c = v * s; // Chroma
  float h_normalized = h / 60.0f; // Hue Sector
  float x = c * (1 - abs(fmod(h_normalized, 2) - 1));  // intermediate value
  float m = v - c;

  float r_prime, g_prime, b_prime;

  if (0 <= h_normalized && h_normalized < 1) {
    r_prime = c; g_prime = x; b_prime = 0;
  }
  else if (1 <= h_normalized && h_normalized < 2) {
    r_prime = x; g_prime = c; b_prime = 0;
  }
  else if (2 <= h_normalized && h_normalized < 3) {
    r_prime = 0; g_prime = c; b_prime = x;
  }
  else if (3 <= h_normalized && h_normalized < 4) {
    r_prime = 0; g_prime = x; b_prime;
  }
  else if (4 <= h_normalized && h_normalized < 5) {
    r_prime = x; g_prime = 0; b_prime = c;
  }
  else {
    r_prime = c; g_prime = 0; b_prime = x;
  }

  return Color { 
   static_cast<uint8>((r_prime + m) * 255),
    static_cast<uint8>((g_prime + m) * 255),
    static_cast<uint8>((b_prime + m) * 255)
  };
}

void printHelp()
{
  std::cout << "Usage: ./paint_contour -i <input_image> -n <number_of_contours> -o <output_image>\n"
            << "  -i <input_image>         Specify the input image file (required)\n"
            << "  -n <number_of_contours>  Specify the number of contours to be displayed (required)\n"
            << "  -o <output_image>        Specify the output image file (required)\n"
            << "-h, --help                 Display this help message";
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
  using morphotree::I32Point;
  using morphotree::AreaComputer;

  using morphotree::extractCountors;
  using morphotree::reconstructContourImage;

  // ====================================================
  // PROCESS COMMAND LINE ARGUMENTS
  // ====================================================
  for (int i = 0; i < argc; i++) {
    std::string arg{ argv[i] };
    if (arg == "-h" || arg == "--help") {
      printHelp();
      return 0;
    }      
  }

  if (argc < 7) {
    std::cerr << "Error: invalid command line argument\n";    
    printHelp();
    return -1;
  }

  // ====================================================
  // PROCESS COMMAND LINE ARGUMENTS
  // ====================================================
  Args args = extractCommandArgs(argc, argv);

  int width, height, nchannels;
  uint8 *data = stbi_load(args.inImage.c_str(), &width, &height, &nchannels, 1);

  Box domain = Box::fromSize({static_cast<uint32>(width), static_cast<uint32>(height)});
  std::vector<uint8> f = std::vector<uint8>(data, data + domain.numberOfPoints());

  std::vector<uint8> outputContour(domain.numberOfPoints()*3);
  std::vector<uint8> nodeImg(domain.numberOfPoints() * 3);

  MTree tree = buildMaxTree(f, std::make_unique<Adjacency8C>(domain));

  if (args.area >= 0) {
    std::vector<uint32> area = std::make_unique<AreaComputer<uint8>>()->computeAttribute(tree);
    tree.idirectFilter([&area, &args](NodePtr node) { return area[node->id()] > args.area; });
  }
  
  std::vector<uint8> f_filtered = tree.reconstructImage();
  I32Point p;
  for (p.y() = domain.top(); p.y() <= domain.bottom(); p.y()++) {
    for (p.x() = domain.left(); p.x() <= domain.right(); p.x()++) {
      uint32 pidx = domain.pointToIndex(p);
      uint32 cpidx = pidx * 3;           // (color) for 3 channels;

      outputContour[cpidx] = f_filtered[pidx];
      outputContour[cpidx+1] = f_filtered[pidx];
      outputContour[cpidx+2] = f_filtered[pidx];

      nodeImg[cpidx] = f_filtered[pidx];
      nodeImg[cpidx+1] = f_filtered[pidx];
      nodeImg[cpidx+2] = f_filtered[pidx];
    }
  }


  std::vector<std::unordered_set<uint32>> contours = 
    extractCountors(domain, f, std::make_shared<InfAdjacency4C>(domain), tree);


  int maxChildren = 0;
  uint32 nodeId = 0;

  std::vector<NodePtr> selectedNodes;
  
  // val_001
  // std::vector<I32Point> points = {
  //   {131, 302}, {191, 304}, {248, 307}
  // };

  // val_188
  std::vector<I32Point> points = {
    {135, 623}, {176, 622}, {238, 619}, {294, 622}, {362, 246}, {31, 664}
  };

  for (auto &p : points) {
    selectedNodes.push_back(tree.smallComponent(domain.pointToIndex(p)));
  }

  // tree.traverseByLevel([&maxChildren, &nodeId, &args, &selectedNodes, &points](NodePtr node){ 
  //   //if (nodeId == 0) {
  //     // if (node->children().size() >= args.ncontours) {
  //     //   maxChildren = node->children().size();
  //     //   nodeId = node->id();
  //     // }
  //     // if (node->children().size() >= maxChildren) {
  //     //   maxChildren = node->children().size();
  //     //   nodeId = node->id();
  //     // }
  //   //}

  //   if (100 <= node->level() && node->level() <= 110)
  //     selectedNodes.push_back(node);
  // });

  std::cout << "\n" << nodeId << " : " << maxChildren << "\n";

  NodePtr node = tree.node(nodeId);
  int cmIdx = 0;                            // colour map index
  for (NodePtr c : selectedNodes) {
    // paint contours

    float hue = (static_cast<float>(cmIdx) / static_cast<float>(maxChildren)) * 360.0f; // hue in degrees
    Color color = HSVtoRGB(hue, SATURATION, VALUE);   // Convert to RGB

    for (uint32 pidx : contours[c->id()]) {
      uint32 cidx = pidx * 3;
      outputContour[cidx] = color.r;
      outputContour[cidx+1] = color.g;
      outputContour[cidx+2] = color.b;
    }

    for (uint32 pidx : c->reconstruct()) {
      uint32 cidx = pidx * 3;
      nodeImg[cidx] = color.r;
      nodeImg[cidx+1] = color.g;
      nodeImg[cidx+2] = color.b;
    }

    cmIdx++;
  }

  std::string contourImageFile = args.outImage;
  contourImageFile.insert(contourImageFile.size()-4, "-contour");
  stbi_write_png(contourImageFile.c_str(), width, height, 3, outputContour.data(), 0);

  std::string nodeImageFile = args.outImage;
  nodeImageFile.insert(nodeImageFile.size()-4, "-node");
  stbi_write_png(nodeImageFile.c_str(), width, height, 3, nodeImg.data(), 0);

  std::cout << "number of nodes: " << tree.numberOfNodes() << ", number of displayed nodes: " << selectedNodes.size() << "\n";

  return 0;
}