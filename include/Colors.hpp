#pragma once
#include "Math.hpp"

using Color = Vec3f;

namespace Colors {
inline Color RED = {1.0f, 0.0f, 0.0f};
inline Color GREEN = {0.0f, 1.0f, 0.0f};
inline Color BLUE = {0.0f, 0.0f, 1.0f};
inline Color BLACK = {0.0f, 0.0f, 0.0f};
inline Color WHITE = {1.0f, 1.0f, 1.0f};
} // namespace Colors
// O lazyvim se recusa a me deixar salvar o arquivo caso eu altere o comentário
// de cima, ele simplesmente desfaz minhas alterações e volta com ele, não
// entendo, mas fazer o que né;
