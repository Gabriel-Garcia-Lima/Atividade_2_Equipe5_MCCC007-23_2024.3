#include "cube.hpp"
#include <iostream>

#include <glm/gtx/fast_trigonometry.hpp>
#include <unordered_map>

// Explicit specialization of std::hash for Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Cube::createBuffers() {
  // Delete previous buffers
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);

  // VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices.at(0)) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // EBO
  abcg::glGenBuffers(1, &m_EBO);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(m_indices.at(0)) * m_indices.size(),
                     m_indices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Cube::loadObj(std::string_view path) {
  tinyobj::ObjReader reader;

  if (!reader.ParseFromFile(path.data())) {
    if (!reader.Error().empty()) {
      throw abcg::RuntimeError(
          fmt::format("Failed to load model {} ({})", path, reader.Error()));
    }
    throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
  }

  if (!reader.Warning().empty()) {
    fmt::print("Warning: {}\n", reader.Warning());
  }

  auto const &attrib{reader.GetAttrib()};
  auto const &shapes{reader.GetShapes()};

  m_vertices.clear();
  m_indices.clear();

  // A key:value map with key=Vertex and value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop over shapes
  for (auto const &shape : shapes) {
    // Loop over indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Access to vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Vertex position
      auto const startIndex{3 * index.vertex_index};
      glm::vec3 position{attrib.vertices.at(startIndex + 0),
                         attrib.vertices.at(startIndex + 1),
                         attrib.vertices.at(startIndex + 2)};

      Vertex const vertex{.position = position};

      // If hash doesn't contain this vertex
      if (!hash.contains(vertex)) {
        // Add this index (size of m_vertices)
        hash[vertex] = m_vertices.size();
        // Add this vertex
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }

  createBuffers();
}

void Cube::paint() {

  // Set uniform variables for the cube
  m_positionMatrix = glm::translate(glm::mat4{1.0f}, m_position);
  m_modelMatrix = m_positionMatrix * m_animationMatrix;
  // Ajuste para transformar o cubo em um prisma retangular
  m_modelMatrix = glm::scale(m_modelMatrix, glm::vec3(m_scale, m_scale, m_scale));
  abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);
  abcg::glUniform4f(m_colorLoc, 0.36f, 0.26f, 0.56f, 0.8f); // Cor

  abcg::glBindVertexArray(m_VAO);

  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

  abcg::glBindVertexArray(0);
}

void Cube::create(GLuint program, GLint modelMatrixLoc, GLint colorLoc,
                  glm::mat4 viewMatrix, float scale,
                  int N) {
  // Release previous VAO
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Create VAO
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);

  // Bind EBO and VBO
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  // Bind vertex attributes
  auto const positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};
  if (positionAttribute >= 0) {
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), nullptr);
  }

  // End of binding
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);

  m_modelMatrixLoc = modelMatrixLoc;
  m_viewMatrix = viewMatrix;
  m_colorLoc = colorLoc;
  m_scale = scale;
  m_maxPos = m_scale * N;
  m_position = glm::vec3(0.0f, m_scale / 2.0f, 0.0f);
}

void Cube::update(float deltaTime) { move(deltaTime); }

void Cube::destroy() const {
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Cube::move(float deltaTime) {
  if (!m_isMoving)
    return;
  float max_angle = 90.0f;

  if (m_angle < max_angle) {
    m_angle += deltaTime * m_angleVelocity;
    if (m_angle > max_angle)
      m_angle = max_angle;

    // Ajuste na animação para considerar o novo tamanho do prisma
    float rotationRadius = m_scale * 1.0f; // Raio de rotação ajustado
    glm::vec3 rotationAxis{0.0f, 0.0f, 0.0f};
    glm::vec3 pivotPoint{0.0f, 0.0f, 0.0f};

    switch (m_orientation) {
    case Orientation::UP:
      rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
      pivotPoint = glm::vec3(0.0f, -rotationRadius, -rotationRadius);
      break;
    case Orientation::DOWN:
      rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
      pivotPoint = glm::vec3(0.0f, -rotationRadius, rotationRadius);
      break;
    case Orientation::LEFT:
      rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
      pivotPoint = glm::vec3(-rotationRadius, -rotationRadius, 0.0f);
      break;
    case Orientation::RIGHT:
      rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
      pivotPoint = glm::vec3(rotationRadius, -rotationRadius, 0.0f);
      break;
    }

    m_animationMatrix = glm::translate(glm::mat4(1.0f), pivotPoint);
    m_animationMatrix = glm::rotate(m_animationMatrix, glm::radians(m_angle), rotationAxis);
    m_animationMatrix = glm::translate(m_animationMatrix, -pivotPoint);

  } else {
    translate();
    resetAnimation();
  }
}

void Cube::resetAnimation() {
  m_animationMatrix = glm::mat4{1.0f};
  m_angle = 0.0f;
  m_isMoving = false;
  m_border = false;
}

void Cube::translate() {
  float moveDistance = m_scale; // Considera o tamanho do prisma

  switch (m_orientation) {
  case Orientation::UP:
    m_position.z -= moveDistance;
    break;
  case Orientation::DOWN:
    m_position.z += moveDistance;
    break;
  case Orientation::LEFT:
    m_position.x -= moveDistance;
    break;
  case Orientation::RIGHT:
    m_position.x += moveDistance;
    break;
  }
}

void Cube::moveUp() {
  if (m_isMoving)
    return;

  if (m_position.z - (m_scale * 2.0f) < -m_maxPos)
    m_border = true;

  m_isMoving = true;
  m_orientation = Orientation::UP;
}

void Cube::moveDown() {
  if (m_isMoving)
    return;

  if (m_position.z + (m_scale * 2.0f) > m_maxPos)
    m_border = true;

  m_isMoving = true;
  m_orientation = Orientation::DOWN;
}

void Cube::moveLeft() {
  if (m_isMoving)
    return;

  if (m_position.x - (m_scale * 2.0f) < -m_maxPos)
    m_border = true;

  m_isMoving = true;
  m_orientation = Orientation::LEFT;
}

void Cube::moveRight() {
  if (m_isMoving)
    return;

  if (m_position.x + (m_scale * 2.0f) > m_maxPos)
    m_border = true;

  m_isMoving = true;
  m_orientation = Orientation::RIGHT;
}
