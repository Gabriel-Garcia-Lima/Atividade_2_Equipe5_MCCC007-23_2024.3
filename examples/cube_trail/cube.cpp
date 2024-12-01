#include "cube.hpp"
#include <iostream>

#include <glm/gtx/fast_trigonometry.hpp>
#include <unordered_map>

// Especialização explícita de std::hash para Vertex
template <> struct std::hash<Vertex> {
  size_t operator()(Vertex const &vertex) const noexcept {
    auto const h1{std::hash<glm::vec3>()(vertex.position)};
    return h1;
  }
};

void Cube::createBuffers() {
  // Deleta buffers anteriores
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

  // Um mapa key:value com key=Vertex e value=index
  std::unordered_map<Vertex, GLuint> hash{};

  // Loop sobre shapes
  for (auto const &shape : shapes) {
    // Loop sobre indices
    for (auto const offset : iter::range(shape.mesh.indices.size())) {
      // Acesso ao vertex
      auto const index{shape.mesh.indices.at(offset)};

      // Posição do vértice
      auto const startIndex{3 * index.vertex_index};
      glm::vec3 position{attrib.vertices.at(startIndex + 0),
                         attrib.vertices.at(startIndex + 1),
                         attrib.vertices.at(startIndex + 2)};

      Vertex const vertex{.position = position};

      // Se hash não contém este vértice
      if (!hash.contains(vertex)) {
        // Adiciona este índice (tamanho de m_vertices)
        hash[vertex] = m_vertices.size();
        // Adiciona este vértice
        m_vertices.push_back(vertex);
      }

      m_indices.push_back(hash[vertex]);
    }
  }

  createBuffers();
}

void Cube::paint() {
  // Configura as variáveis uniformes para o cubo
  m_positionMatrix = glm::translate(glm::mat4{1.0f}, m_position);
  m_modelMatrix = m_positionMatrix * m_animationMatrix;

  // Ajusta a escala do prisma com base no estado
  glm::vec3 scaleVec{m_scale, m_scale * 2.0f, m_scale};

  if (m_state == State::LAYING_Z) {
    scaleVec.z *= 2.0f;
    scaleVec.y = m_scale;
  } else if (m_state == State::LAYING_X) {
    scaleVec.x *= 2.0f;
    scaleVec.y = m_scale;
  }

  m_modelMatrix = glm::scale(m_modelMatrix, scaleVec);

  abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &m_modelMatrix[0][0]);
  abcg::glUniform4f(m_colorLoc, 0.36f, 0.26f, 0.56f, 0.8f); // Cor

  abcg::glBindVertexArray(m_VAO);

  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);

  // Renderizar as bordas no modo wireframe
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Ativar modo wireframe
  abcg::glUniform4f(m_colorLoc, 0.0f, 0.0f, 0.0f, 1.0f); // Cor das arestas (preto)
  abcg::glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Voltar ao modo preenchido

  abcg::glBindVertexArray(0);
}

void Cube::create(GLuint program, GLint modelMatrixLoc, GLint colorLoc,
                  glm::mat4 viewMatrix, float scale,
                  int N) {
  // Libera o VAO anterior
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Cria o VAO
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);

  // Vincula EBO e VBO
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

  // Vincula os atributos de vértice
  auto const positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};
  if (positionAttribute >= 0) {
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), nullptr);
  }

  // Fim da vinculação
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);

  m_modelMatrixLoc = modelMatrixLoc;
  m_viewMatrix = viewMatrix;
  m_colorLoc = colorLoc;
  m_scale = scale;
  m_maxPos = m_scale * N;
}

void Cube::update(float deltaTime) { move(deltaTime); }

void Cube::destroy() const {
  abcg::glDeleteBuffers(1, &m_EBO);
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Cube::move(float deltaTime) {
  if (!m_isMoving && !m_isFalling)
    return;

  if (m_isFalling) {
    // Animação de queda
    m_fallTime += deltaTime;
    m_position.y -= m_fallSpeed * deltaTime;
    if (m_fallTime > m_fallDuration) {
      resetGame();
    }
    return;
  }

  float max_angle = 90.0f;

  if (m_angle < max_angle) {
    m_angle += deltaTime * m_angleVelocity;
    if (m_angle > max_angle)
      m_angle = max_angle;

    glm::vec3 rotationAxis{0.0f, 0.0f, 0.0f};
    glm::vec3 pivotPoint{0.0f, 0.0f, 0.0f};

    float offset = m_scale / 2.0f;

    // Ajustar o eixo de rotação e o ponto de pivô com base no estado e orientação
    if (m_state == State::STANDING) {
      if (m_orientation == Orientation::UP || m_orientation == Orientation::DOWN) {
        rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
        pivotPoint = m_position + glm::vec3(0.0f, -offset, m_orientation == Orientation::UP ? -offset : offset);
      } else {
        rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
        pivotPoint = m_position + glm::vec3(m_orientation == Orientation::LEFT ? -offset : offset, -offset, 0.0f);
      }
    } else if (m_state == State::LAYING_Z) {
      rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
      offset = m_scale;
      pivotPoint = m_position + glm::vec3(0.0f, -offset, m_orientation == Orientation::UP ? -offset : offset);
    } else if (m_state == State::LAYING_X) {
      rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
      offset = m_scale;
      pivotPoint = m_position + glm::vec3(m_orientation == Orientation::LEFT ? -offset : offset, -offset, 0.0f);
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
  float moveDistance = m_scale;

  switch (m_state) {
  case State::STANDING:
    if (m_orientation == Orientation::UP) {
      m_position.z -= moveDistance;
    } else if (m_orientation == Orientation::DOWN) {
      m_position.z += moveDistance;
    } else if (m_orientation == Orientation::LEFT) {
      m_position.x -= moveDistance;
    } else if (m_orientation == Orientation::RIGHT) {
      m_position.x += moveDistance;
    }
    break;

  case State::LAYING_Z:
    if (m_orientation == Orientation::UP || m_orientation == Orientation::DOWN) {
      m_position.z += (m_orientation == Orientation::UP ? -2.0f : 2.0f) * moveDistance;
    } else {
      m_position.x += (m_orientation == Orientation::LEFT ? -1.0f : 1.0f) * moveDistance;
    }
    break;

  case State::LAYING_X:
    if (m_orientation == Orientation::LEFT || m_orientation == Orientation::RIGHT) {
      m_position.x += (m_orientation == Orientation::LEFT ? -2.0f : 2.0f) * moveDistance;
    } else {
      m_position.z += (m_orientation == Orientation::UP ? -1.0f : 1.0f) * moveDistance;
    }
    break;
  }

  // Atualizar o estado do prisma após a translação
  if (m_state == State::STANDING) {
    if (m_orientation == Orientation::UP || m_orientation == Orientation::DOWN) {
      m_state = State::LAYING_Z;
    } else {
      m_state = State::LAYING_X;
    }
  } else if (m_state == State::LAYING_Z || m_state == State::LAYING_X) {
    m_state = State::STANDING;
  }
}

void Cube::moveUp() {
  if (m_isMoving || m_isFalling)
    return;

  bool willFall = false;

  switch (m_state) {
  case State::STANDING:
    willFall = (m_position.z - (m_scale * 1.0f) < -m_maxPos);
    break;
  case State::LAYING_Z:
    willFall = (m_position.z - (m_scale * 2.0f) < -m_maxPos);
    break;
  case State::LAYING_X:
    willFall = (m_position.z - (m_scale * 1.0f) < -m_maxPos);
    break;
  }

  if (willFall) {
    m_border = true;
    m_isFalling = true;
    m_fallTime = 0.0f;
    return;
  }

  m_isMoving = true;
  m_orientation = Orientation::UP;
}

void Cube::moveDown() {
  if (m_isMoving || m_isFalling)
    return;

  bool willFall = false;

  switch (m_state) {
  case State::STANDING:
    willFall = (m_position.z + (m_scale * 1.0f) > m_maxPos);
    break;
  case State::LAYING_Z:
    willFall = (m_position.z + (m_scale * 2.0f) > m_maxPos);
    break;
  case State::LAYING_X:
    willFall = (m_position.z + (m_scale * 1.0f) > m_maxPos);
    break;
  }

  if (willFall) {
    m_border = true;
    m_isFalling = true;
    m_fallTime = 0.0f;
    return;
  }

  m_isMoving = true;
  m_orientation = Orientation::DOWN;
}

void Cube::moveLeft() {
  if (m_isMoving || m_isFalling)
    return;

  bool willFall = false;

  switch (m_state) {
  case State::STANDING:
    willFall = (m_position.x - (m_scale * 1.0f) < -m_maxPos);
    break;
  case State::LAYING_X:
    willFall = (m_position.x - (m_scale * 2.0f) < -m_maxPos);
    break;
  case State::LAYING_Z:
    willFall = (m_position.x - (m_scale * 1.0f) < -m_maxPos);
    break;
  }

  if (willFall) {
    m_border = true;
    m_isFalling = true;
    m_fallTime = 0.0f;
    return;
  }

  m_isMoving = true;
  m_orientation = Orientation::LEFT;
}

void Cube::moveRight() {
  if (m_isMoving || m_isFalling)
    return;

  bool willFall = false;

  switch (m_state) {
  case State::STANDING:
    willFall = (m_position.x + (m_scale * 1.0f) > m_maxPos);
    break;
  case State::LAYING_X:
    willFall = (m_position.x + (m_scale * 2.0f) > m_maxPos);
    break;
  case State::LAYING_Z:
    willFall = (m_position.x + (m_scale * 1.0f) > m_maxPos);
    break;
  }

  if (willFall) {
    m_border = true;
    m_isFalling = true;
    m_fallTime = 0.0f;
    return;
  }

  m_isMoving = true;
  m_orientation = Orientation::RIGHT;
}

void Cube::resetGame() {
  m_position = glm::vec3(0.0f, 0.0f, 0.0f);
  m_state = State::STANDING;
  m_isMoving = false;
  m_isFalling = false;
  m_animationMatrix = glm::mat4{1.0f};
  m_angle = 0.0f;
  m_border = false;
  m_fallTime = 0.0f;
}
