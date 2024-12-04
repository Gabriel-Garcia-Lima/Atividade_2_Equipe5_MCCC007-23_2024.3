#ifndef GROUND_HPP_
#define GROUND_HPP_

#include "abcgOpenGL.hpp"
#include "vertex.hpp"
#include <vector>


class Ground {
public:
  void create(GLuint program, GLint modelMatrixLoc, GLint colorLoc, float scale, int N);
  void paint();
  void destroy();

  // Add functions to manage the hole
  void setHole(int x, int z);
  bool isTile(int x, int z) const;
  void getHolePosition(int& x, int& z) const;

private:
  std::vector<Vertex> m_vertices;
  float m_scale;
  int m_N; // The grid size will be (2N+1) x (2N+1)
  GLuint m_VAO{};
  GLuint m_VBO{};

  GLint m_modelMatrixLoc{};
  GLint m_colorLoc{};

  // 2D vector to represent the grid
  std::vector<std::vector<bool>> m_grid;

  // Coordinates of the hole
  int m_holeX{-1};
  int m_holeZ{-1};
};

#endif
