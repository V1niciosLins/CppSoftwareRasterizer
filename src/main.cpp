
#include "Framebuffer.hpp"
#include "Math.hpp"
#include "Texture.hpp"
#include "mesh.hpp"
#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cout << "Argumentos insuficientes. Diga o caminho para o arquivo e se "
                 "tiver, da textura."
              << std::endl;
    return -1;
  } else if (argc > 3) {
    std::cout << "Muitos argumentos. Diga apenas o caminho para o arquivo e da "
                 "textura, se tiver"
              << std::endl;
    return -1;
  }
  int width = 700;
  int height = 500;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << "Falha no SDL_Init: " << SDL_GetError() << '\n';
    return -1;
  }

  SDL_Window *window =
      SDL_CreateWindow("Rasterizador Software", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  SDL_Texture *screen_texture =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING, width, height);

  Framebuffer fb{width, height};

  Texture texture;
  // Se argc ==3 ent eu passei a textura; Caso não, tenta ler o mtl;
  if (argc == 3) {
    if (!texture.load_from_file(argv[2])) {
    }
  }

  bool running = true;
  SDL_Event event;

  Mesh mesh;
  if (!mesh.load_from_obj(argv[1])) {
    return -1;
  }

  float angle{0.0f};
  Vec3f cam_pos = {0.0f, 40.0f, -120.0f};
  const Uint8 *keys = SDL_GetKeyboardState(NULL);

  while (running) {
    angle += 0.01f;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        running = false;
      }
    }
    float speed = 2.0f;
    if (keys[SDL_SCANCODE_W])
      cam_pos.z += speed;
    if (keys[SDL_SCANCODE_S])
      cam_pos.z -= speed;
    if (keys[SDL_SCANCODE_A])
      cam_pos.x -= speed;
    if (keys[SDL_SCANCODE_D])
      cam_pos.x += speed;
    if (keys[SDL_SCANCODE_SPACE])
      cam_pos.y += speed; // Sobe
    if (keys[SDL_SCANCODE_LSHIFT])
      cam_pos.y -= speed; // Desce
    //
    Mat4x4 proj =
        Mat4x4::projection(90.0f * (3.14159f / 180.0f),
                           static_cast<float>(width) / height, 0.1f, 1000.0f);

    // Matriz View: Empurra o mundo na direção NEGATIVA da câmera
    Mat4x4 view = Mat4x4::translation(-cam_pos.x, -cam_pos.y, -cam_pos.z);

    Mat4x4 model = Mat4x4::rotation_y(angle);

    Mat4x4 mvp = proj * view * model;
    // Lembrar: O fb.clear() não apenas preenche o buffer de cores com uma cor,
    // ele injeta infinity() no buffer profundidade;
    fb.clear({1.0f, 1.0f, 0.1f});

    for (size_t i = 0; i < mesh.vertices.size(); i += 3) {

      Vertex vert0 = mesh.vertices[i];
      Vertex vert1 = mesh.vertices[i + 1];
      Vertex vert2 = mesh.vertices[i + 2];

      Vec4f v0 = {vert0.pos.x, vert0.pos.y, vert0.pos.z, 1.0f};
      Vec4f v1 = {vert1.pos.x, vert1.pos.y, vert1.pos.z, 1.0f};
      Vec4f v2 = {vert2.pos.x, vert2.pos.y, vert2.pos.z, 1.0f};

      v0 = v0 * mvp;
      v0.perspective_divide();
      v1 = v1 * mvp;
      v1.perspective_divide();
      v2 = v2 * mvp;
      v2.perspective_divide();

      v0.x = (v0.x + 1.0f) * 0.5f * width;
      v0.y = (1.0f - v0.y) * 0.5f * height;
      v1.x = (v1.x + 1.0f) * 0.5f * width;
      v1.y = (1.0f - v1.y) * 0.5f * height;
      v2.x = (v2.x + 1.0f) * 0.5f * width;
      v2.y = (1.0f - v2.y) * 0.5f * height;

      float edge1_x = v1.x - v0.x;
      float edge1_y = v1.y - v0.y;
      float edge2_x = v2.x - v0.x;
      float edge2_y = v2.y - v0.y;
      float area = (edge1_x * edge2_y) - (edge1_y * edge2_x);

      if (area <= 0.0f)
        continue;

      // Se não houver textura, faz fallback pro mtl
      if (!texture.pixels.empty()) {
        fb.draw_triangle(
            {.pos = {v0.x, v0.y, v0.z}, .uv = {vert0.uv.u(), vert0.uv.v()}},
            {.pos = {v1.x, v1.y, v1.z},
             .uv = {vert1.uv.u(),
                    vert1.uv.v()}}, // Winding Order corrigido aqui
            {.pos = {v2.x, v2.y, v2.z}, .uv = {vert2.uv.u(), vert2.uv.v()}},
            texture.pixels, texture.width, texture.height);
      } else {
        fb.draw_triangle({.pos = {v0.x, v0.y, v0.z},
                          .uv = {vert0.uv.u(), vert0.uv.v()},
                          .color = vert0.color},
                         {.pos = {v1.x, v1.y, v1.z},
                          .uv = {vert1.uv.u(), vert1.uv.v()},
                          .color = vert1.color},
                         {.pos = {v2.x, v2.y, v2.z},
                          .uv = {vert2.uv.u(), vert2.uv.v()},
                          .color = vert2.color});
      }
    }
    fb.update_hardware_buffer();

    SDL_UpdateTexture(screen_texture, nullptr, fb.hardware_buffer_.data(),
                      width * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, screen_texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
  }

  // Devolve a RAM pro SO
  SDL_DestroyTexture(screen_texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
