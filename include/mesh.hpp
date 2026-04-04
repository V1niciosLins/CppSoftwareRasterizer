#pragma once
#include "Math.hpp"
#include <string>
#include <unordered_map>
#include <vector>

struct Material {
  Vec3f Kd{1.0f, 1.0f, 1.0f};
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::unordered_map<std::string, Material> materials;

  bool load_from_obj(const std::string &filename);
  bool load_mtl(const std::string &filename);
};
