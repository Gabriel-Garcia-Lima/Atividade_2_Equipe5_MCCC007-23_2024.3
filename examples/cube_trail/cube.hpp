#ifndef CUBE_HPP_
#define CUBE_HPP_

#include "abcgOpenGL.hpp"
#include "vertex.hpp"
#include "ground.hpp"
#include <random>

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
  void setGround(Ground* ground);
  bool isOnHole() const;
  void setTexture(GLuint texture) { m_texture = texture; };


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

  // Novo membro para geração aleatória de posição
  std::mt19937 m_randomEngine{std::random_device{}()};
  std::uniform_int_distribution<int> m_xDistribution{0, 3};
  std::uniform_int_distribution<int> m_zDistribution{0, 3};

  void move(float deltaTime);
  void translate();
  void resetAnimation();
  
  // Novo método para gerar posição aleatória
  glm::vec3 generateRandomPosition();

  int m_rotationDirection{1}; 

  Ground* m_ground{nullptr};
  GLuint m_texture{0};
};

#endif