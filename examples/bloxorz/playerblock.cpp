// playerblock.cpp

#include "playerblock.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

void PlayerBlock::initialize(GLuint program, glm::ivec2 const &startPosition) {
  // Inicializa a posição e a orientação do bloco
  m_position = glm::vec3(startPosition.x + 0.5f, 0.5f, startPosition.y + 0.5f);
  m_orientation = Orientation::Standing;
  m_moveCount = 0;

  // Carrega o modelo do bloco (prisma retangular)
  float width = 1.0f;
  float height = 2.0f;
  float depth = 1.0f;

  float halfWidth = width / 2.0f;
  float halfHeight = height / 2.0f;
  float halfDepth = depth / 2.0f;

  // Vértices do bloco
  std::vector<float> vertices = {
      // Posições          // Normais           // Coordenadas de textura
      // Frente
      -halfWidth, -halfHeight, halfDepth,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
       halfWidth, -halfHeight, halfDepth,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
       halfWidth,  halfHeight, halfDepth,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
      -halfWidth,  halfHeight, halfDepth,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
      // Traseira
      -halfWidth, -halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
       halfWidth, -halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
       halfWidth,  halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
      -halfWidth,  halfHeight, -halfDepth,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
      // Esquerda
      -halfWidth, -halfHeight, -halfDepth, -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
      -halfWidth, -halfHeight,  halfDepth, -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
      -halfWidth,  halfHeight,  halfDepth, -1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
      -halfWidth,  halfHeight, -halfDepth, -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
      // Direita
       halfWidth, -halfHeight, -halfDepth,  1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
       halfWidth, -halfHeight,  halfDepth,  1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
       halfWidth,  halfHeight,  halfDepth,  1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
       halfWidth,  halfHeight, -halfDepth,  1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
      // Topo
      -halfWidth,  halfHeight,  halfDepth,  0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
       halfWidth,  halfHeight,  halfDepth,  0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
       halfWidth,  halfHeight, -halfDepth,  0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
      -halfWidth,  halfHeight, -halfDepth,  0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
      // Base
      -halfWidth, -halfHeight,  halfDepth,  0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
       halfWidth, -halfHeight,  halfDepth,  0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
       halfWidth, -halfHeight, -halfDepth,  0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
      -halfWidth, -halfHeight, -halfDepth,  0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
  };

  // Índices do bloco
  std::vector<GLuint> indices = {
      0, 1, 2, 2, 3, 0,         // Frente
      4, 5, 6, 6, 7, 4,         // Traseira
      8, 9, 10, 10, 11, 8,      // Esquerda
      12, 13, 14, 14, 15, 12,   // Direita
      16, 17, 18, 18, 19, 16,   // Topo
      20, 21, 22, 22, 23, 20    // Base
  };

  // Configurar VAO, VBO e EBO
  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  glGenBuffers(1, &m_VBO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &m_EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

  // Atributos de vértice (posição, normal, coordenadas de textura)
  GLint positionAttribute = 0;
  GLint normalAttribute = 1;
  GLint texCoordAttribute = 2;

  // Posições
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);

  // Normais
  glEnableVertexAttribArray(normalAttribute);
  glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));

  // Coordenadas de textura
  glEnableVertexAttribArray(texCoordAttribute);
  glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));

  glBindVertexArray(0);

  m_indicesCount = static_cast<GLsizei>(indices.size());
}

void PlayerBlock::update(float deltaTime, Level const &level) {
  if (m_isMoving) {
    // Atualiza a animação de movimento
    m_animationTime += deltaTime;
    float t = m_animationTime / m_animationDuration;
    if (t >= 1.0f) {
      t = 1.0f;
      m_isMoving = false;
      m_position = m_endPosition;
      // Atualiza a orientação do bloco
      m_startAngle = m_endAngle;
    }
    // Interpola a posição e a rotação
    float angle = glm::mix(m_startAngle, m_endAngle, t);
    m_position = glm::mix(m_startPosition, m_endPosition, t);
    // Atualiza a rotação (não é necessário armazenar)
  } else if (!m_moveQueue.empty()) {
    // Inicia um novo movimento
    Direction dir = m_moveQueue.front();
    m_moveQueue.pop();

    // Calcula a nova posição e orientação
    glm::vec3 deltaPos(0.0f);
    glm::vec3 rotationAxis(0.0f);
    float rotationAngle = glm::radians(90.0f);

    glm::ivec2 gridPos(static_cast<int>(m_position.x), static_cast<int>(m_position.z));

    switch (dir) {
      case Direction::Up:
        deltaPos.z -= 1.0f;
        rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);
        break;
      case Direction::Down:
        deltaPos.z += 1.0f;
        rotationAxis = glm::vec3(-1.0f, 0.0f, 0.0f);
        break;
      case Direction::Left:
        deltaPos.x -= 1.0f;
        rotationAxis = glm::vec3(0.0f, 0.0f, -1.0f);
        break;
      case Direction::Right:
        deltaPos.x += 1.0f;
        rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);
        break;
    }

    // Calcula a nova posição e orientação com base no estado atual
    glm::vec3 newPosition = m_position + deltaPos;
    Orientation newOrientation = m_orientation;

    // Atualiza a orientação
    if (m_orientation == Orientation::Standing) {
      newOrientation = (dir == Direction::Up || dir == Direction::Down) ? Orientation::LyingZ : Orientation::LyingX;
    } else if (m_orientation == Orientation::LyingX && (dir == Direction::Left || dir == Direction::Right)) {
      newOrientation = Orientation::Standing;
    } else if (m_orientation == Orientation::LyingZ && (dir == Direction::Up || dir == Direction::Down)) {
      newOrientation = Orientation::Standing;
    } else {
      // Continua deitado, desloca mais uma unidade
      newPosition += deltaPos;
    }

    // Verifica colisões com o nível
    glm::ivec2 pos1, pos2;
    if (newOrientation == Orientation::Standing) {
      pos1 = glm::ivec2(newPosition.x, newPosition.z);
      pos2 = pos1;
    } else if (newOrientation == Orientation::LyingX) {
      pos1 = glm::ivec2(newPosition.x - 0.5f, newPosition.z);
      pos2 = glm::ivec2(newPosition.x + 0.5f, newPosition.z);
    } else { // LyingZ
      pos1 = glm::ivec2(newPosition.x, newPosition.z - 0.5f);
      pos2 = glm::ivec2(newPosition.x, newPosition.z + 0.5f);
    }

    // Verifica se as posições estão em tiles válidos
    if (level.getTileType(pos1.x, pos1.y) == 0 || level.getTileType(pos2.x, pos2.y) == 0) {
      // Movimento inválido
      return;
    }

    // Se o movimento for válido, inicia a animação
    m_isMoving = true;
    m_animationTime = 0.0f;
    m_startPosition = m_position;
    m_endPosition = newPosition;
    m_rotationAxis = rotationAxis;
    m_startAngle = 0.0f;
    m_endAngle = rotationAngle;
    m_orientation = newOrientation;
    m_moveCount++;
  }
}

void PlayerBlock::render(GLuint program) const {
  // Renderiza o bloco do jogador
  glBindVertexArray(m_VAO);

  // Calcula a matriz modelo
  glm::mat4 modelMatrix = glm::mat4(1.0f);

  modelMatrix = glm::translate(modelMatrix, m_position - glm::vec3(0.0f, 0.5f, 0.0f));

  if (m_isMoving) {
    float t = m_animationTime / m_animationDuration;
    float angle = glm::mix(m_startAngle, m_endAngle, t);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.5f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, angle, m_rotationAxis);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -0.5f, 0.0f));
  }

  // Envia a matriz modelo para o shader
  GLint modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  // Define a cor do material (por exemplo, azul)
  GLint objectColorLoc = glGetUniformLocation(program, "objectColor");
  glm::vec3 objectColor = glm::vec3(0.0f, 0.5f, 1.0f);
  glUniform3fv(objectColorLoc, 1, &objectColor[0]);

  glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
}

void PlayerBlock::destroy() {
  glDeleteBuffers(1, &m_VBO);
  glDeleteBuffers(1, &m_EBO);
  glDeleteVertexArrays(1, &m_VAO);
}

void PlayerBlock::queueMove(Direction dir) {
  m_moveQueue.push(dir);
}

void PlayerBlock::reset(glm::ivec2 const &startPosition) {
  m_position = glm::vec3(startPosition.x + 0.5f, 0.5f, startPosition.y + 0.5f);
  m_orientation = Orientation::Standing;
  m_moveCount = 0;
  while (!m_moveQueue.empty()) m_moveQueue.pop();
  m_isMoving = false;
}

bool PlayerBlock::hasWon(Level const &level) const {
  // Verifica se o bloco está na posição de vitória e na orientação correta
  // Para simplificar, vamos supor que o tile com valor 3 é o objetivo
  glm::ivec2 pos(static_cast<int>(m_position.x), static_cast<int>(m_position.z));
  if (level.getTileType(pos.x, pos.y) == 3 && m_orientation == Orientation::Standing && !m_isMoving) {
    return true;
  }
  return false;
}

bool PlayerBlock::hasLost(Level const &level) const {
  // Verifica se o bloco caiu do tabuleiro ou em uma posição inválida
  glm::ivec2 pos1, pos2;
  if (m_orientation == Orientation::Standing) {
    pos1 = glm::ivec2(m_position.x, m_position.z);
    pos2 = pos1;
  } else if (m_orientation == Orientation::LyingX) {
    pos1 = glm::ivec2(m_position.x - 0.5f, m_position.z);
    pos2 = glm::ivec2(m_position.x + 0.5f, m_position.z);
  } else { // LyingZ
    pos1 = glm::ivec2(m_position.x, m_position.z - 0.5f);
    pos2 = glm::ivec2(m_position.x, m_position.z + 0.5f);
  }

  if (level.getTileType(pos1.x, pos1.y) == 0 || level.getTileType(pos2.x, pos2.y) == 0) {
    return true;
  }
  return false;
}
