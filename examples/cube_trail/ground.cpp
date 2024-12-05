#include "ground.hpp"
#include <random>

void Ground::create(GLuint program, GLint modelMatrixLoc, GLint colorLoc, float scale, int N) {
  // Define um quadrado unitário no plano xz
  m_vertices = {{
    {.position = {+0.5f, 0.0f, -0.5f}}, // Vértice 1
    {.position = {-0.5f, 0.0f, -0.5f}}, // Vértice 2
    {.position = {+0.5f, 0.0f, +0.5f}}, // Vértice 3
    {.position = {-0.5f, 0.0f, +0.5f}}  // Vértice 4
  }};

  // VBO
  abcg::glGenBuffers(1, &m_VBO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  abcg::glBufferData(GL_ARRAY_BUFFER,
                     sizeof(m_vertices.at(0)) * m_vertices.size(),
                     m_vertices.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Cria VAO e vincula os atributos de vértice
  abcg::glGenVertexArrays(1, &m_VAO);
  abcg::glBindVertexArray(m_VAO);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  // Vincula os atributos de vértice
  auto const positionAttribute{
      abcg::glGetAttribLocation(program, "inPosition")};
  if (positionAttribute >= 0) {
    abcg::glEnableVertexAttribArray(positionAttribute);
    abcg::glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE,
                                sizeof(Vertex), nullptr);
  }

  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
  abcg::glBindVertexArray(0);

  // Initialize the grid with all tiles present
  m_N = N;
  m_scale = scale;
  m_grid.resize(2 * m_N + 1, std::vector<bool>(2 * m_N + 1, true));

  // Randomize hole position on creation
  randomizeHole();

  // Carrega a localização das variáveis uniformes do shader
  m_modelMatrixLoc = modelMatrixLoc;
  m_colorLoc = colorLoc;
}

void Ground::paint() {
  abcg::glBindVertexArray(m_VAO);

  for (auto const z : iter::range(-m_N, m_N + 1)) {
    for (auto const x : iter::range(-m_N, m_N + 1)) {
      // Skip drawing if the tile is a hole
      if (!isTile(x, z)) continue;

      glm::mat4 model{1.0f};

      model = glm::translate(model, glm::vec3(x * m_scale, 0.0f, z * m_scale));
      model = glm::scale(model, glm::vec3(m_scale, m_scale, m_scale));

      abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);

      // Define color (checkerboard pattern)
      auto const gray{(z + x) % 2 == 0 ? 0.5f : 1.0f};
      abcg::glUniform4f(m_colorLoc, gray, gray, gray, 1.0f);

      abcg::glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
  }

  abcg::glBindVertexArray(0);
}

void Ground::destroy() {
  abcg::glDeleteBuffers(1, &m_VBO);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Ground::setHole(int x, int z) {
  // Convert grid coordinates to vector indices
  int gridX = x + m_N;
  int gridZ = z + m_N;

  if (gridX >= 0 && gridX < 2 * m_N + 1 && gridZ >= 0 && gridZ < 2 * m_N + 1) {
    m_grid[gridX][gridZ] = false; // Set tile as hole
    m_holeX = x;
    m_holeZ = z;
  }
}

bool Ground::isTile(int x, int z) const {
  int gridX = x + m_N;
  int gridZ = z + m_N;
  if (gridX >= 0 && gridX < 2 * m_N + 1 && gridZ >= 0 && gridZ < 2 * m_N + 1) {
    return m_grid[gridX][gridZ];
  }
  return false; // Outside grid bounds
}

void Ground::getHolePosition(int& x, int& z) const {
  x = m_holeX;
  z = m_holeZ;
}

void Ground::randomizeHole() {
  // Use a uniform distribution to randomly select hole position
  std::uniform_int_distribution<> xDist(-m_N, m_N);
  std::uniform_int_distribution<> zDist(-m_N, m_N);

  int newHoleX, newHoleZ;
  do {
    newHoleX = xDist(m_gen);
    newHoleZ = zDist(m_gen);
  } while (newHoleX == 0 && newHoleZ == 0); // Avoid placing hole at the center

  // Clear previous grid and set hole
  m_grid = std::vector<std::vector<bool>>(2 * m_N + 1, std::vector<bool>(2 * m_N + 1, true));
  setHole(newHoleX, newHoleZ);
}

bool Ground::isGameOver() const {
  return !isTile(m_holeX, m_holeZ);
}

void Ground::reset() {
  // Reset grid and randomize hole
  randomizeHole();
}