project(performace_contour)

include_directories(include)

# CMAKE VERSION
cmake_minimum_required(VERSION 3.10)

# C++ VERSION
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# CONAN CONFIG
# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup()

find_package(morphotree REQUIRED)
find_package(stb REQUIRED)

add_executable(perf_incr_contour perf_incr_contour.cpp)
target_link_libraries(perf_incr_contour 
  morphotree::morphotree
  stb::stb)

add_executable(perf_incr_contour_hashmap perf_incr_contour_hashmap.cpp)
target_link_libraries(perf_incr_contour_hashmap 
  morphotree::morphotree
  stb::stb)

add_executable(perf_incr_contour_red_black_tree perf_incr_contour_red_black_tree.cpp)
target_link_libraries(perf_incr_contour_red_black_tree 
  morphotree::morphotree
  stb::stb)

add_executable(perf_non_incr_contour perf_non_incr_contour.cpp)
target_link_libraries(perf_non_incr_contour 
  morphotree::morphotree
  stb::stb)
  
add_executable(perf_contour_trace perf_contour_trace.cpp)
target_link_libraries(perf_contour_trace 
  morphotree::morphotree
  stb::stb)

add_executable(check_contour_algorithms check_contour_algorithms.cpp)
target_link_libraries(check_contour_algorithms 
  morphotree::morphotree
  stb::stb)

add_executable(end_slides_image end_slides_image.cpp)
target_link_libraries(end_slides_image 
  morphotree::morphotree
  stb::stb)