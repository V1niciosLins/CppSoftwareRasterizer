#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <sstream>

bool Mesh::load_from_obj(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open())
    return false;

  std::vector<Vec3f> temp_positions;
  std::vector<Vec2f> temp_uvs;

  std::string current_mtl = "";
  std::string base_dir = filename.substr(0, filename.find_last_of('/') + 1);

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string type;
    iss >> type;

    if (type == "v") {
      Vec3f pos;
      iss >> pos.x >> pos.y >> pos.z;
      temp_positions.push_back(pos);
    } else if (type == "vt") {
      Vec2f uv;
      iss >> uv.u() >> uv.v();

      temp_uvs.push_back(uv);
    } else if (type == "f") {
      std::string vertex_data;
      std::vector<Vertex> face_vertices;

      while (iss >> vertex_data) {
        int pos_idx = -1, uv_idx = -1;

        size_t first_slash = vertex_data.find('/');
        if (first_slash != std::string::npos) {
          pos_idx = std::stoi(vertex_data.substr(0, first_slash)) - 1;

          size_t second_slash = vertex_data.find('/', first_slash + 1);
          if (second_slash != std::string::npos) {
            if (second_slash > first_slash + 1) { // Tem UV no meio (v/vt/vn)
              uv_idx = std::stoi(vertex_data.substr(
                           first_slash + 1, second_slash - first_slash - 1)) -
                       1;
            }
          } else { // Tem só v/vt
            uv_idx = std::stoi(vertex_data.substr(first_slash + 1)) - 1;
          }
        } else {
          pos_idx = std::stoi(vertex_data) - 1; // Só tem posição (v)
        }

        Vertex vert;
        vert.pos = temp_positions[pos_idx];

        if (!current_mtl.empty() && materials.count(current_mtl)) {
          vert.color = materials[current_mtl].Kd;
        }

        if (uv_idx >= 0 && uv_idx < static_cast<int>(temp_uvs.size())) {
          vert.uv = temp_uvs[uv_idx];
        } else {
          vert.uv = {0.0f,
                     0.0f}; // Fallback de segurança se o modelo for quebrado
        }
        face_vertices.push_back(vert);
      }

      if (face_vertices.size() >= 3) {
        for (size_t i = 1; i < face_vertices.size() - 1; ++i) {
          vertices.push_back(face_vertices[0]);     // Pivô
          vertices.push_back(face_vertices[i]);     // Atual
          vertices.push_back(face_vertices[i + 1]); // Próximo
        }
      }
    } else if (type == "mtllib") {
      std::string mtl_filename;
      iss >> mtl_filename;
      load_mtl(base_dir + mtl_filename);
    } else if (type == "usemtl") {
      iss >> current_mtl;
    }
  }
  std::cout << "Modelo carregado: " << (vertices.size() / 3)
            << " triângulos flat na memoria.\n";
  return true;
}

bool Mesh::load_mtl(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open())
    return false;

  std::string line, current_mtl;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string type;
    iss >> type;

    if (type == "newmtl") {

      std::getline(iss >> std::ws, current_mtl);
      if (!current_mtl.empty() && current_mtl.back() == '\r')
        current_mtl.pop_back();

    } else if (type == "Kd" && !current_mtl.empty()) {
      iss >> materials[current_mtl].Kd.x >> materials[current_mtl].Kd.y >>
          materials[current_mtl].Kd.z;
    }
  }
  return true;
}
