#ifndef GROUND_HPP_
#define GROUND_HPP_

#include "abcgOpenGL.hpp"
#include "vertex.hpp"
#include <vector>
#include <random>

class Ground {
public:
  void create(GLuint program, GLint modelMatrixLoc, GLint colorLoc, float scale, int N);
  void paint();
  void destroy();

  // Add functions to manage the hole
  void setHole(int x, int z);
  bool isTile(int x, int z) const;
  void getHolePosition(int& x, int& z) const;
  
  // New methods
  void randomizeHole(); // Método para gerar buraco aleatório
  bool isGameOver() const; // Verifica se o jogo terminou
  void reset(); // Reinicia o estado do chão

  int getHoleX() const { return m_holeX; }
  int getHoleZ() const { return m_holeZ; }
  int getN() const { return m_N; }


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

  // Random number generator for hole placement
  std::random_device m_rd;
  std::mt19937 m_gen{m_rd()};

  
};

#endif