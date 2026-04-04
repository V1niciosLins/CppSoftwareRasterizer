#pragma once
#include <cmath>
// Lembrar: Pra coordenadas espaciais acessar direto com .x ou .y ou .z;
// para cores: .r() .g() .b();
//
// o vesmo é válido para o Vec2;

template <typename T> struct Vector2 {
  T x;
  T y;

  inline T &u() { return x; }
  inline T &v() { return y; }
  inline const T &u() const { return x; }
  inline const T &v() const { return y; }

  Vector2<T> operator+(Vector2<T> other) const {
    return {x + other.x, y + other.y};
  }
  Vector2<T> operator*(T value) const { return {x * value, y * value}; }
};

template <typename T> struct Vector3 {
  T x;
  T y;
  T z;

  inline T &r() { return x; }
  inline T &g() { return y; }
  inline T &b() { return z; }
  inline const T &r() const { return x; }
  inline const T &g() const { return y; }
  inline const T &b() const { return z; }

  Vector3<T> operator+(Vector3<T> other) const {
    return {x + other.x, y + other.y, z + other.z};
  }
  Vector3<T> operator*(T value) const {
    return {x * value, y * value, z * value};
  }
  Vector3<T> operator*(Vector3<T> other) const {
    return {x * other.x, y * other.y, z * other.z};
  };
};
struct Mat4x4 {
  float m_[16]{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  static Mat4x4 identity() {
    return {1.0f, 0, 0, 0, 0, 1.0f, 0, 0, 0, 0, 1.0f, 0, 0, 0, 0, 1.0f};
  }

  static Mat4x4 projection(float fov, float aspect_ratio, float near,
                           float far) {
    Mat4x4 m;
    m.m_[0] = 1.0f / (aspect_ratio * std::tan(fov / 2.0f));
    m.m_[5] = 1.0f / (std::tan(fov / 2.0f));
    m.m_[10] = far / (far - near);
    m.m_[11] = (-far * near) / (far - near);
    m.m_[14] = 1.0f;
    m.m_[15] = 0.0f;
    return m;
  }

  static Mat4x4 translation(float tx, float ty, float tz) {
    Mat4x4 m = identity();
    m.m_[3] = tx;
    m.m_[7] = ty;
    m.m_[11] = tz;
    return m;
  }

  static Mat4x4 rotation_x(float angle_rad) {
    Mat4x4 mat = {};
    float c = std::cos(angle_rad);
    float s = std::sin(angle_rad);

    // m_[linha * 4 + coluna]
    mat.m_[0] = 1.0f;  // linha 0, coluna 0
    mat.m_[5] = c;     // linha 1, coluna 1
    mat.m_[6] = -s;    // linha 1, coluna 2
    mat.m_[9] = s;     // linha 2, coluna 1
    mat.m_[10] = c;    // linha 2, coluna 2
    mat.m_[15] = 1.0f; // linha 3, coluna 3

    return mat;
  }

  static Mat4x4 rotation_y(float angle_rad) {
    Mat4x4 mat = {};
    float c = std::cos(angle_rad);
    float s = std::sin(angle_rad);

    mat.m_[0] = c;     // linha 0, coluna 0
    mat.m_[2] = s;     // linha 0, coluna 2
    mat.m_[5] = 1.0f;  // linha 1, coluna 1
    mat.m_[8] = -s;    // linha 2, coluna 0
    mat.m_[10] = c;    // linha 2, coluna 2
    mat.m_[15] = 1.0f; // linha 3, coluna 3

    return mat;
  }

  static Mat4x4 rotation_z(float angle_rad) {
    Mat4x4 mat = {};
    float c = std::cos(angle_rad);
    float s = std::sin(angle_rad);

    mat.m_[0] = c;     // linha 0, coluna 0
    mat.m_[1] = -s;    // linha 0, coluna 1
    mat.m_[4] = s;     // linha 1, coluna 0
    mat.m_[5] = c;     // linha 1, coluna 1
    mat.m_[10] = 1.0f; // linha 2, coluna 2
    mat.m_[15] = 1.0f; // linha 3, coluna 3

    return mat;
  }

  Mat4x4 operator*(const Mat4x4 &rhs) const {
    Mat4x4 res;
    for (int r = 0; r < 4; ++r) {
      for (int c = 0; c < 4; ++c) {
        res.m_[r * 4 + c] = (m_[r * 4 + 0] * rhs.m_[0 * 4 + c]) +
                            (m_[r * 4 + 1] * rhs.m_[1 * 4 + c]) +
                            (m_[r * 4 + 2] * rhs.m_[2 * 4 + c]) +
                            (m_[r * 4 + 3] * rhs.m_[3 * 4 + c]);
      }
    }
    return res;
  }
};
struct Vec4f {
  float x, y, z, w{1.0f};

  Vec4f operator*(const Mat4x4 &mat) const {
    return {
        ((mat.m_[0] * x) + (mat.m_[1] * y) + (mat.m_[2] * z) + (mat.m_[3] * w)),
        ((mat.m_[4] * x) + (mat.m_[5] * y) + (mat.m_[6] * z) + (mat.m_[7] * w)),
        ((mat.m_[8] * x) + (mat.m_[9] * y) + (mat.m_[10] * z) +
         (mat.m_[11] * w)),
        ((mat.m_[12] * x) + (mat.m_[13] * y) + (mat.m_[14] * z) +
         (mat.m_[15] * w))};
    ;
  }

  void perspective_divide() {
    if (w != 0.0f) {
      x /= w;
      y /= w;
      z /= w;
    }
  }
};

using Vec2f = Vector2<float>;
using Vec2i = Vector2<int>;
using Vec3f = Vector3<float>;

struct Vertex {
  Vec3f pos;
  Vec2f uv{0.0f, 0.0f};
  Vec3f color{1.0f, 1.0f, 1.0f};
};
