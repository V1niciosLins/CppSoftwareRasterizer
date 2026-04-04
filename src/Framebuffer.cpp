#include "Framebuffer.hpp"
#include "Colors.hpp"
#include "Math.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <string_view>

// No ATO de INSTANCIAR O FRAMEBUFFER, o COLORS já é preenchido com ZEROS;
Framebuffer::Framebuffer(int width, int height)
    : width_{width}, height_{height}, colors_(height * width),
      depth_buffer_(height * width), hardware_buffer_(width * height) {}

void Framebuffer::set_pixel(Vec2i coords, const Vector3<float> &color) {
  if (coords.x < 0 || coords.x >= width_ || coords.y < 0 || coords.y >= height_)
    return;

  int index = (coords.y * width_) + coords.x;

  colors_[index] = color;
}

void Framebuffer::draw_rectangle(Vec2i coords, int w, int h,
                                 const Color &color) {
  int start_x = std::max(0, coords.x);
  int start_y = std::max(0, coords.y);

  int end_x = std::min(width_, coords.x + w);
  int end_y = std::min(height_, coords.y + h);

  for (int i = start_y; i < end_y; ++i) {
    int row = (i * width_);
    for (int p = start_x; p < end_x; ++p) {
      colors_[row + p] = color;
    }
  }
}

void Framebuffer::draw_line(const Vertex &vertex0, const Vertex &vertex1,
                            const Color &color) {
  Vec2i pos = {static_cast<int>(vertex0.pos.x),
               static_cast<int>(vertex0.pos.y)};
  int dx = std::abs(vertex1.pos.x - vertex0.pos.x);
  int dy = -std::abs(vertex1.pos.y - vertex0.pos.y);

  int sx = vertex0.pos.x < vertex1.pos.x ? 1 : -1;
  int sy = vertex0.pos.y < vertex1.pos.y ? 1 : -1;
  int err = dx + dy;

  while (1) {
    set_pixel(pos, color);

    if (pos.x == vertex1.pos.x && pos.y == vertex1.pos.y)
      break;

    int e2 = err << 1;

    if (e2 >= dy) {
      err += dy;
      pos.x += sx;
    }

    if (e2 <= dx) {
      err += dx;
      pos.y += sy;
    }
  }
}

void Framebuffer::draw_triangle(const Vertex &vertex0, const Vertex &vertex1,
                                const Vertex &vertex2) {
  float triangle_area = edge_cross(vertex0.pos.x, vertex0.pos.y, vertex1.pos.x,
                                   vertex1.pos.y, vertex2.pos.x, vertex2.pos.y);

  int min_x =
      std::max(0.0f, std::min({vertex0.pos.x, vertex1.pos.x, vertex2.pos.x}));
  int max_x = std::min(width_ - 1.0f,
                       std::max({vertex0.pos.x, vertex1.pos.x, vertex2.pos.x}));

  int min_y =
      std::max(0.0f, std::min({vertex0.pos.y, vertex1.pos.y, vertex2.pos.y}));
  int max_y = std::min(height_ - 1.0f,
                       std::max({vertex0.pos.y, vertex1.pos.y, vertex2.pos.y}));

  for (int y = min_y; y <= max_y; ++y) {
    int row = (y * width_);
    for (int x = min_x; x <= max_x; ++x) {
      float w0 = edge_cross(x + 0.5f, y + 0.5f, vertex1.pos.x, vertex1.pos.y,
                            vertex2.pos.x, vertex2.pos.y);
      float w1 = edge_cross(x + 0.5f, y + 0.5f, vertex2.pos.x, vertex2.pos.y,
                            vertex0.pos.x, vertex0.pos.y);
      float w2 = edge_cross(x + 0.5f, y + 0.5f, vertex0.pos.x, vertex0.pos.y,
                            vertex1.pos.x, vertex1.pos.y);

      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        float alpha = w0 / triangle_area;
        float beta = w1 / triangle_area;
        float gamma = 1.0f - alpha - beta;

        float z_pixel = (vertex0.pos.z * alpha) + (vertex1.pos.z * beta) +
                        (vertex2.pos.z * gamma);
        if (z_pixel < depth_buffer_[row + x]) {
          depth_buffer_[row + x] = z_pixel;
          colors_[row + x] = (vertex0.color * alpha) + (vertex1.color * beta) +
                             (vertex2.color * gamma);
        }
      }
    }
  }
}

void Framebuffer::draw_triangle(const Vertex &vertex0, const Vertex &vertex1,
                                const Vertex &vertex2,
                                const std::vector<Color> &texture,
                                int tex_width, int tex_height) {
  float triangle_area = edge_cross(vertex0.pos.x, vertex0.pos.y, vertex1.pos.x,
                                   vertex1.pos.y, vertex2.pos.x, vertex2.pos.y);

  int min_x =
      std::max(0.0f, std::min({vertex0.pos.x, vertex1.pos.x, vertex2.pos.x}));
  int max_x = std::min(width_ - 1.0f,
                       std::max({vertex0.pos.x, vertex1.pos.x, vertex2.pos.x}));

  int min_y =
      std::max(0.0f, std::min({vertex0.pos.y, vertex1.pos.y, vertex2.pos.y}));
  int max_y = std::min(height_ - 1.0f,
                       std::max({vertex0.pos.y, vertex1.pos.y, vertex2.pos.y}));

#pragma omp parallel for
  for (int y = min_y; y <= max_y; ++y) {
    int row = y * width_;
    for (int x = min_x; x <= max_x; ++x) {
      float w0 = edge_cross(x + 0.5f, y + 0.5f, vertex1.pos.x, vertex1.pos.y,
                            vertex2.pos.x, vertex2.pos.y);
      float w1 = edge_cross(x + 0.5f, y + 0.5f, vertex2.pos.x, vertex2.pos.y,
                            vertex0.pos.x, vertex0.pos.y);
      float w2 = edge_cross(x + 0.5f, y + 0.5f, vertex0.pos.x, vertex0.pos.y,
                            vertex1.pos.x, vertex1.pos.y);

      if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
        float alpha = w0 / triangle_area;
        float beta = w1 / triangle_area;
        float gamma = w2 / triangle_area;

        float z_pixel = (vertex0.pos.z * alpha) + (vertex1.pos.z * beta) +
                        (vertex2.pos.z * gamma);
        if (z_pixel < depth_buffer_[row + x]) {

          float u_pixel = (vertex0.uv.u() * alpha) + (vertex1.uv.u() * beta) +
                          (vertex2.uv.u() * gamma);

          float v_pixel = (vertex0.uv.v() * alpha) + (vertex1.uv.v() * beta) +
                          (vertex2.uv.v() * gamma);

          int tex_x = static_cast<int>(u_pixel * (tex_width - 1));
          int tex_y = static_cast<int>(v_pixel * (tex_height - 1));

          tex_x = std::clamp(tex_x, 0, tex_width - 1);
          tex_y = std::clamp(tex_y, 0, tex_height - 1);

          int texture_index = (tex_y * tex_width) + tex_x;
          Color color_pixel_interpoled = (vertex0.color * alpha) +
                                         (vertex1.color * beta) +
                                         (vertex2.color * gamma);
          depth_buffer_[row + x] = z_pixel;
          colors_[row + x] = texture[texture_index] * color_pixel_interpoled;
        }
      }
    }
  }
}

void Framebuffer::update_hardware_buffer() {
  for (size_t i = 0; i < colors_.size(); ++i) {
    int r = static_cast<int>(colors_[i].r() * 255.999f);
    int g = static_cast<int>(colors_[i].g() * 255.999f);
    int b = static_cast<int>(colors_[i].b() * 255.999f);

    r = std::clamp(r, 0, 255);
    g = std::clamp(g, 0, 255);
    b = std::clamp(b, 0, 255);

    // SDL_PIXELFORMAT_ARGB8888
    hardware_buffer_[i] = (255 << 24) | (r << 16) | (g << 8) | b;
  }
}

void Framebuffer::save_to_ppm(std::string_view path) const {
  std::ofstream ofstream{std::string{path}};
  if (!ofstream)
    return;

  ofstream << "P3\n";
  ofstream << width_ << " " << height_ << '\n';
  ofstream << 255 << '\n';

  for (const Color &color : colors_) {
    int r = static_cast<int>(color.r() * 255.999f);
    int g = static_cast<int>(color.g() * 255.999f);
    int b = static_cast<int>(color.b() * 255.999f);

    r = r > 255 ? 255 : (r < 0 ? 0 : r);
    g = g > 255 ? 255 : (g < 0 ? 0 : g);
    b = b > 255 ? 255 : (b < 0 ? 0 : b);

    ofstream << r << " " << g << " " << b << '\n';
  }
}
