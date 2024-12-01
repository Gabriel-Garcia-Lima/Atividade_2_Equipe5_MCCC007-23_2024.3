// level.hpp

#ifndef LEVEL_HPP_
#define LEVEL_HPP_

#include <string>
#include <vector>

#include "abcg.hpp"
#include "glm/glm.hpp"

class Level {
public:
  void loadFromFile(std::string const &filename);
  void render(GLuint program) const;
  void destroy();

  int getCurrentLevel() const { return m_currentLevel; }
  glm::ivec2 getStartPosition() const { return m_startPosition; }

  int getTileType(int x, int y) const;
  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }

private:
  int m_currentLevel{1};
  std::vector<std::vector<int>> m_layout;
  glm::ivec2 m_startPosition{0, 0};
  int m_width{0};
  int m_height{0};

  // OpenGL objects
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  // Número de índices para desenhar os tiles
  GLsizei m_indicesCount{};
};

#endif
