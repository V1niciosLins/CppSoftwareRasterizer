#pragma once
#include "Colors.hpp"
#include "Math.hpp"
#include <cstdint>
#include <limits>
#include <string_view>
#include <vector>

class Framebuffer {
  int width_;
  int height_;
  std::vector<Color> colors_;

  // inline int flatten(int x, int y) const { return (y * width_) + x; }
  inline float edge_cross(float px, float py, float x0, float y0, float x1,
                          float y1) const {
    return (x1 - x0) * (py - y0) - (y1 - y0) * (px - x0);
  }

public:
  std::vector<float> depth_buffer_;
  std::vector<uint32_t> hardware_buffer_;

  Framebuffer(int width, int height);
  void set_pixel(Vec2i coords, const Color &color = Colors::RED);
  void draw_rectangle(Vec2i coords, int w, int h,
                      const Color &color = Colors::RED);
  // Aqui eu to usando o algoritmo de Bresenham, sinceramente, não sei se
  // estudei corretamente mas vou tentar;
  void draw_line(const Vertex &vertex0, const Vertex &vertex1,
                 const Color &color = Colors::RED);

  inline void clear(const Color &color) {
    std::fill(colors_.begin(), colors_.end(), color);
    std::fill(depth_buffer_.begin(), depth_buffer_.end(),
              std::numeric_limits<float>::infinity());
  }
  void draw_triangle(const Vertex &vertex0, const Vertex &vertex1,
                     const Vertex &vertex2);

  void draw_triangle(const Vertex &v0, const Vertex &v1, const Vertex &v2,
                     const std::vector<Color> &texture, int tex_width,
                     int tex_height);

  void update_hardware_buffer();
  void save_to_ppm(std::string_view path) const;
};
