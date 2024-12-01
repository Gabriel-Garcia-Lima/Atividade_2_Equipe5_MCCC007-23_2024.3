// level.cpp

#include "level.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

void Level::loadFromFile(std::string const &filename) {
  // Carrega o layout do nível a partir de um arquivo
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Não foi possível abrir o arquivo de nível: " << filename << std::endl;
    return;
  }

  m_layout.clear();

  std::string line;
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::vector<int> row;
    int tile;
    while (ss >> tile) {
      row.push_back(tile);
    }
    m_layout.push_back(row);
  }

  file.close();

  m_height = static_cast<int>(m_layout.size());
  m_width = m_height > 0 ? static_cast<int>(m_layout[0].size()) : 0;

  // Encontra a posição inicial do bloco (assumindo que o valor 2 representa o início)
  for (int y = 0; y < m_height; ++y) {
    for (int x = 0; x < m_width; ++x) {
      if (m_layout[y][x] == 2) {
        m_startPosition = glm::ivec2(x, y);
        m_layout[y][x] = 1; // Marca como um tile normal
        break;
      }
    }
  }

  // Configura os buffers OpenGL para os tiles
  std::vector<float> vertices;
  std::vector<GLuint> indices;

  // Gerar os dados dos tiles com base em m_layout
  GLuint indexOffset = 0;

  for (int y = 0; y < m_height; ++y) {
    for (int x = 0; x < m_width; ++x) {
      int tileType = m_layout[y][x];
      if (tileType > 0) {
        // Adiciona vértices para o tile
        float tileSize = 1.0f;
        float tileHeight = 0.1f; // Espessura do tile

        float x0 = x * tileSize;
        float y0 = -tileHeight;
        float z0 = y * tileSize;

        // Vértices do cubo achatado (tile)
        std::vector<float> tileVertices = {
            // Posições          // Normais           // Coordenadas de textura
            // Topo
            x0,         0.0f,    z0 + tileSize,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
            x0 + tileSize, 0.0f, z0 + tileSize,  0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
            x0 + tileSize, 0.0f, z0,            0.0f, 1.0f, 0.0f,   1.0f, 1.0f,
            x0,         0.0f,    z0,            0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
            // Base
            x0,         y0,      z0 + tileSize,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
            x0 + tileSize, y0,   z0 + tileSize,  0.0f, -1.0f, 0.0f,  1.0f, 0.0f,
            x0 + tileSize, y0,   z0,            0.0f, -1.0f, 0.0f,  1.0f, 1.0f,
            x0,         y0,      z0,            0.0f, -1.0f, 0.0f,  0.0f, 1.0f,
        };

        // Adiciona os vértices
        vertices.insert(vertices.end(), tileVertices.begin(), tileVertices.end());

        // Índices
        std::vector<GLuint> tileIndices = {
            // Topo
            indexOffset, indexOffset + 1, indexOffset + 2,
            indexOffset + 2, indexOffset + 3, indexOffset,
            // Base
            indexOffset + 4, indexOffset + 5, indexOffset + 6,
            indexOffset + 6, indexOffset + 7, indexOffset + 4,
            // Lados podem ser adicionados se necessário
        };

        // Adiciona os índices
        indices.insert(indices.end(), tileIndices.begin(), tileIndices.end());

        indexOffset += 8; // 8 vértices por tile
      }
    }
  }

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

void Level::render(GLuint program) const {
  // Renderiza os tiles do nível
  glBindVertexArray(m_VAO);

  // Define a cor do material (por exemplo, cinza)
  GLint objectColorLoc = glGetUniformLocation(program, "objectColor");
  glm::vec3 objectColor = glm::vec3(0.6f, 0.6f, 0.6f);
  glUniform3fv(objectColorLoc, 1, &objectColor[0]);

  // Envia a matriz modelo (identidade neste caso)
  GLint modelMatrixLoc = glGetUniformLocation(program, "modelMatrix");
  glm::mat4 modelMatrix = glm::mat4(1.0f);
  glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

  glDrawElements(GL_TRIANGLES, m_indicesCount, GL_UNSIGNED_INT, nullptr);

  glBindVertexArray(0);
}

void Level::destroy() {
  glDeleteBuffers(1, &m_VBO);
  glDeleteBuffers(1, &m_EBO);
  glDeleteVertexArrays(1, &m_VAO);
}

int Level::getTileType(int x, int y) const {
  if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
    return m_layout[y][x];
  }
  return 0; // Fora do tabuleiro
}
