#include "graph/graph.hpp"
#include "graph/library.hpp"
#include "graph/properties.hpp"

#include <boost/graph/graphviz.hpp>
#include <boost/property_map/dynamic_property_map.hpp>

#include <filesystem>
#include <iostream>

using namespace utils;

std::string dir_path = "./dot_output";

// Helper function to write a graph to a file in the dot format
void write_to_file(const std::string &filename, const gl::GraphBundle &gb) {

  auto &g = gb.graph;

  // Create the output directory if it doesn't exist.
  if (!std::filesystem::exists(dir_path)) {
    bool success = std::filesystem::create_directory(dir_path);
    if (!success) {
      std::cerr << "Could not create directory " << dir_path << std::endl;
      exit(1);
    }
  }

  boost::dynamic_properties dp;
  dp.property("node_id",
              boost::get(boost::vertex_index, g)); // This line is compulsory

  // Create maps for all the main graphviz features
  gl::VertexMap<std::string> positionMap; // Position of vertices.
  gl::VertexMap<std::string>
      pinMap; // Boolean string saying the vertex positions should be fixed.
  gl::VertexMap<std::string> shapeMap; // Shape of vertices.
  gl::VertexMap<std::string> labelMap; // Label of vertices.

  gl::VertexMap<std::vector<double>> position = gb.props.vertex["position"];
  for (auto [v, pos] : position) {
    positionMap[v] =
        std::to_string(pos[0]) + ',' + std::to_string(pos[1]) + '!';
    pinMap[v] = "true";
    shapeMap[v] = "circle";
    labelMap[v] = std::to_string(v);
  }

  // Add the maps to the dynamic properties object.
  dp.property("pos", boost::make_assoc_property_map(positionMap));
  dp.property("pin", boost::make_assoc_property_map(pinMap));
  dp.property("shape", boost::make_assoc_property_map(shapeMap));
  dp.property("label", boost::make_assoc_property_map(labelMap));

  // Open an output stream to the file
  std::ofstream ofs(dir_path + "/" + filename);

  // The function that actually writes the graph to the file.
  boost::write_graphviz_dp(ofs, g, dp);
}

int main() {

  auto gb = gl::grid(3, 3);
  write_to_file("grid.dot", gb);

  auto gb2 = gl::complete(5);
  write_to_file("complete.dot", gb2);

  auto gb3 = gl::kuratowski(3, 3);
  write_to_file("kura.dot", gb3);

  auto gb4 = gl::chimera(2, 2, 2);
  write_to_file("chimera.dot", gb4);

  auto gb5 = gl::ring(5);
  write_to_file("ring.dot", gb5);

  auto gb6 = gl::path(5);
  write_to_file("path.dot", gb6);

  auto gb7 = gl::ibm_hex(3, 3);
  write_to_file("ibm_hex.dot", gb7);

  auto gb8 = gl::kagome(4, 4);
  write_to_file("kagome.dot", gb8);

  auto gb9 = gl::random(10, 0.5);
  write_to_file("rand.dot", gb9);

  auto gb10 = gl::random_bipartite(10, 10, 0.5);
  write_to_file("rand_bipartite.dot", gb10);

  auto gb11 = gl::random_regular(10, 3);
  write_to_file("rand_regular.dot", gb11);
}
