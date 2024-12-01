#ifndef CUBE_HPP_
#define CUBE_HPP_

#include "abcgOpenGL.hpp"
#include "vertex.hpp"

class Cube {
public:
  void loadObj(std::string_view path);
  void paint();
  void update(float deltaTime);
  void create(GLuint program, GLint modelMatrixLoc, GLint colorLoc, glm::mat4 viewMatrix, float scale, int N);
  void destroy() const;
  void moveLeft();
  void moveRight();
  void moveUp();
  void moveDown();
  void resetGame();

private:
  GLuint m_VAO{};
  GLuint m_VBO{};
  GLuint m_EBO{};
  GLuint m_EBOEdges{};

  glm::mat4 m_animationMatrix{1.0f};
  glm::mat4 m_viewMatrix;
  glm::mat4 m_positionMatrix{1.0f};
  glm::mat4 m_modelMatrix{1.0f};
  GLint m_modelMatrixLoc;

  GLint m_colorLoc;

  std::vector<Vertex> m_vertices;
  std::vector<GLuint> m_indices;
  std::vector<GLuint> m_edgeIndices;

  void createBuffers();

  enum class Orientation { DOWN, RIGHT, UP, LEFT };
  enum class State { STANDING, LAYING_X, LAYING_Z };

  glm::vec3 m_position{};
  float m_scale{1.0f};
  float m_angle{};
  Orientation m_orientation{Orientation::DOWN};
  State m_state{State::STANDING};

  bool m_isMoving{false};
  float m_maxPos{1.0f};
  float m_angleVelocity{360.0f};
  bool m_border{false};

  bool m_isFalling{false};
  float m_fallTime{0.0f};
  float m_fallDuration{1.0f};
  float m_fallSpeed{2.0f};

  void move(float deltaTime);
  void translate();
  void resetAnimation();
};

#endif
