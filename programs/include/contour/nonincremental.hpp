#pragma once

#include <morphotree/core/box.hpp>
#include <morphotree/core/alias.hpp>
#include <morphotree/adjacency/adjacency.hpp>


#include <vector>

std::vector<bool> computeContourNonIncremental(std::shared_ptr<morphotree::Adjacency> adj,
  morphotree::Box &domain, 
  const std::vector<bool> &bimg)
{
  using morphotree::I32Point;
  using morphotree::uint32;

  std::vector<bool> contour(domain.numberOfPoints(), false);

  for (uint32 pidx = 0; pidx < domain.numberOfPoints(); pidx++) {
    for (uint32 qidx : adj->neighbours(pidx)) {
      if (bimg[pidx] && 
        (!domain.contains(domain.indexToPoint(qidx)) || !bimg[qidx])) {
        contour[pidx] = true;
      }
    }
  }
  
  return contour;
}