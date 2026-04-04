#define STB_IMAGE_IMPLEMENTATION
#include "Texture.hpp"
#include "stb_image.h"
#include <iostream>

bool Texture::load_from_file(const std::string &path) {
  int channels;

  stbi_set_flip_vertically_on_load(true);

  unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 3);

  if (!data) {
    std::cerr << "Erro fatal: Nao foi possivel carregar a textura: " << path
              << "\n";
    return false;
  }

  pixels.resize(width * height);

  for (int i = 0; i < width * height; ++i) {

    float r = data[i * 3 + 0] / 255.0f;
    float g = data[i * 3 + 1] / 255.0f;
    float b = data[i * 3 + 2] / 255.0f;
    pixels[i] = {r, g, b};
  }

  stbi_image_free(data);

  std::cout << "Textura carregada: " << width << "x" << height << "\n";
  return true;
}
