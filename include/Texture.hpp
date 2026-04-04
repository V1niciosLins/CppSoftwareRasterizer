#pragma once
#include "Colors.hpp"
#include <string>
#include <vector>

struct Texture {
  int width = 0;
  int height = 0;
  std::vector<Color> pixels;

  bool load_from_file(const std::string &path);
};
