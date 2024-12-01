// playerblock.hpp

#ifndef PLAYERBLOCK_HPP_
#define PLAYERBLOCK_HPP_

#include <glm/glm.hpp>
#include <queue>

#include "abcg.hpp"
#include "direction.hpp"
#include "level.hpp"

enum class Orientation { Standing, LyingX, LyingZ };

class PlayerBlock {
public:
  void initialize(GLuint program, glm::ivec2 const &startPosition);
  void update(float deltaTime, Level const &level);
  void render(GLuint program) const;
  void destroy();

  void queueMove(Direction dir);
  void reset(glm::ivec2 const &startPosition);

  bool hasWon(Level const &level) const;
  bool hasLost(Level const &level) const;

  int getMoveCount() const { return m_moveCount; }

private:
  glm::vec3 m_position;
  Orientation m_orientation;

  int m_moveCount{};

  // Fila de movimentos
  std::queue<Direction> m_moveQueue;

  // OpenGL objects
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};

  // Número de índices para desenhar o bloco
  GLsizei m_indicesCount{};

  // Animação
  bool m_isMoving{false};
  float m_animationTime{};
  float m_animationDuration{0.2f}; // Duração da animação de movimento
  glm::vec3 m_startPosition;
  glm::vec3 m_endPosition;
  glm::vec3 m_rotationAxis;
  float m_startAngle{};
  float m_endAngle{};
};

#endif
