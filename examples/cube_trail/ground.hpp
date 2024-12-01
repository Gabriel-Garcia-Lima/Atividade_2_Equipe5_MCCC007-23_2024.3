#ifndef GROUND_HPP_
#define GROUND_HPP_

#include "abcgOpenGL.hpp"
#include "vertex.hpp"

class Ground {
public:
  void create(GLuint program, GLint modelMatrixLoc, GLint colorLoc, float scale, int N);
  void paint();
  void destroy();

private:
  std::vector<Vertex> m_vertices;
  float m_scale;
  int m_N; // O tamanho da grade será (2N+1) x (2N+1)
  GLuint m_VAO{};
  GLuint m_VBO{};

  GLint m_modelMatrixLoc{};
  GLint m_colorLoc{};
};

#endif
