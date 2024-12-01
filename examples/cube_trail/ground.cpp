#include "ground.hpp"

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

  // Carrega a localização das variáveis uniformes do shader
  m_modelMatrixLoc = modelMatrixLoc;
  m_colorLoc = colorLoc;
  m_scale = scale;
  m_N = N; // Para um plano 7x7, defina N = 3
}

void Ground::paint() {
  abcg::glBindVertexArray(m_VAO);

  for (auto const z : iter::range(-m_N, m_N + 1)) {
    for (auto const x : iter::range(-m_N, m_N + 1)) {
      glm::mat4 model{1.0f};

      model = glm::translate(model, glm::vec3(x * m_scale, 0.0f, z * m_scale));
      model = glm::scale(model, glm::vec3(m_scale, m_scale, m_scale));

      abcg::glUniformMatrix4fv(m_modelMatrixLoc, 1, GL_FALSE, &model[0][0]);

      // Define a cor (padrão xadrez)
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