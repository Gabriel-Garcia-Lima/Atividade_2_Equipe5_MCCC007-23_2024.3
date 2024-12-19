#ifndef VERTEX_HPP_
#define VERTEX_HPP_

#include "abcgOpenGL.hpp"

struct Vertex {
  glm::vec3 position{};
  glm::vec3 normal{};    // Adiciona normais
  glm::vec2 texCoord{};  // Adiciona coordenadas de textura

  friend bool operator==(Vertex const&, Vertex const&) = default;
};

#endif
