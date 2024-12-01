#include "render.hpp"
#include <glm/gtx/rotate_vector.hpp>
#include <vector>

// Renderiza os obstáculos circulares do jogo
void Render::renderObstacles(Window &window, glm::vec2 const &position,
                             float radius) {
  glBindVertexArray(window.m_VAO);

  // Define o número de triângulos para formar o círculo
  static const int numTriangles = 20;
  std::vector<glm::vec2> positions;
  positions.emplace_back(0, 0); // Centro do círculo

  // Gera pontos em arranjo circular para formar o obstáculo
  for (int i = 0; i <= numTriangles; i++) {
    auto const angle = i * M_PI * 2.0f / numTriangles;
    positions.emplace_back(radius * std::cos(angle), radius * std::sin(angle));
  }

  // Define a cor do obstáculo como branco
  glUniform4f(window.m_colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

  // Configura a posição do obstáculo
  glUniform2f(window.m_translateLoc, position.x, position.y);
  glUniform1f(window.m_rotateLoc, 0.0f);
  glUniform1f(window.m_scaleLoc, window.m_gameScale);

  // Cria e configura o buffer de vértices (VBO)
  GLuint VBO{};
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);

  // Desenha o obstáculo usando TRIANGLE_FAN para criar um círculo preenchido
  glDrawArrays(GL_TRIANGLE_FAN, 0, positions.size());

  // Limpa o buffer após uso
  glDeleteBuffers(1, &VBO);
}

// Renderiza os flippers (pás) do pinball
void Render::renderFlipper(Window &window, Flipper const &flipper,
                           bool isLeft) {
  glBindVertexArray(window.m_VAO);

  const float flipperHalfHeight = -0.1f;

  // Define os vértices do flipper baseado em sua orientação (esquerda ou
  // direita)
  std::array<glm::vec2, 4> positions;
  if (isLeft) {
    positions = {
        glm::vec2{0.0f, -flipperHalfHeight},
        glm::vec2{flipper.length / window.m_gameScale, -flipperHalfHeight},
        glm::vec2{flipper.length / window.m_gameScale, flipperHalfHeight},
        glm::vec2{0.0f, flipperHalfHeight}};
  } else {
    positions = {
        glm::vec2{0.0f, -flipperHalfHeight},
        glm::vec2{-flipper.length / window.m_gameScale, -flipperHalfHeight},
        glm::vec2{-flipper.length / window.m_gameScale, flipperHalfHeight},
        glm::vec2{0.0f, flipperHalfHeight}};
  }

  // Configura cor (branco) e transformações do flipper
  glUniform4f(window.m_colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);
  glUniform2f(window.m_translateLoc, flipper.position.x, flipper.position.y);
  float angle = isLeft ? flipper.currentAngle : -flipper.currentAngle;
  glUniform1f(window.m_rotateLoc, angle);
  glUniform1f(window.m_scaleLoc, window.m_gameScale);

  // Cria e configura o buffer de vértices
  GLuint VBO{};
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);

  // Desenha o flipper como um polígono preenchido
  glDrawArrays(GL_TRIANGLE_FAN, 0, positions.size());

  glDeleteBuffers(1, &VBO);
}

// Renderiza a bola do pinball
void Render::renderBall(Window &window) {
  glBindVertexArray(window.m_VAO);

  // Cria uma aproximação circular usando triângulos
  static const int numTriangles = 20;
  std::vector<glm::vec2> positions;
  positions.emplace_back(0, 0); // Centro da bola

  // Gera pontos em círculo para formar a bola
  for (int i = 0; i <= numTriangles; i++) {
    auto const angle = i * M_PI * 2.0f / numTriangles;
    positions.emplace_back(window.m_ball.radius * std::cos(angle),
                           window.m_ball.radius * std::sin(angle));
  }

  // Define a cor da bola como vermelho e configura transformações
  glUniform4f(window.m_colorLoc, 1.0f, 0.0f, 0.0f, 1.0f);
  glUniform2f(window.m_translateLoc, window.m_ball.position.x,
              window.m_ball.position.y);
  glUniform1f(window.m_rotateLoc, 0.0f);
  glUniform1f(window.m_scaleLoc, window.m_gameScale);

  // Configura buffer de vértices
  GLuint VBO{};
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);

  // Desenha a bola como um círculo preenchido
  glDrawArrays(GL_TRIANGLE_FAN, 0, positions.size());

  glDeleteBuffers(1, &VBO);
}

// Renderiza as paredes e limites do campo de jogo
void Render::renderWalls(Window &window) {
  glBindVertexArray(window.m_VAO);

  // Define os vértices das paredes com uma abertura no canto superior direito
  std::vector<glm::vec2> positions{
      // Parede esquerda
      {WALL_LEFT, WALL_BOTTOM},
      {WALL_LEFT + 0.1f, WALL_BOTTOM},
      {WALL_LEFT + 0.1f, WALL_TOP},
      {WALL_LEFT, WALL_TOP},

      // Parede direita com abertura próxima ao topo
      {WALL_RIGHT - 0.1f, WALL_BOTTOM},
      {WALL_RIGHT, WALL_BOTTOM},
      {WALL_RIGHT, WALL_TOP - 0.3f}, // Início da abertura
      {WALL_RIGHT - 0.1f, WALL_TOP - 0.3f},

      // Continuação da parede direita após a abertura
      {WALL_RIGHT - 0.1f, WALL_TOP - 0.1f},
      {WALL_RIGHT, WALL_TOP - 0.1f},

      // Parede superior
      {WALL_LEFT + 0.1f, WALL_TOP - 0.1f},
      {WALL_RIGHT - 0.1f, WALL_TOP - 0.1f},
  };

  // Adiciona seções fixas da parede inferior atrás dos flippers
  positions.push_back(
      {WALL_LEFT, WALL_BOTTOM - 2.0f}); // Início da parede inferior esquerda
  positions.push_back(
      {WALL_LEFT, WALL_BOTTOM + 0.2f}); // Fim da parede inferior esquerda
  positions.push_back(
      {m_leftFlipper.position.x,
       m_leftFlipper.position.y}); // Posição do flipper esquerdo

  positions.push_back(
      {WALL_RIGHT, WALL_BOTTOM - 2.0f}); // Início da parede inferior direita
  positions.push_back(
      {WALL_RIGHT, WALL_BOTTOM + 0.2f}); // Fim da parede inferior direita
  positions.push_back(
      {m_rightFlipper.position.x,
       m_rightFlipper.position.y}); // Posição do flipper direito

  // Configura cor cinza para as paredes e suas transformações
  glUniform4f(window.m_colorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
  glUniform2f(window.m_translateLoc, 0.0f, 0.0f);
  glUniform1f(window.m_rotateLoc, 0.0f);
  glUniform1f(window.m_scaleLoc, 1.0f);

  // Configura buffer de vértices
  GLuint VBO{};
  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
               positions.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
  glEnableVertexAttribArray(0);

  // Desenha cada seção das paredes como um loop de linhas separado
  glDrawArrays(GL_LINE_LOOP, 0, 4); // Parede esquerda
  glDrawArrays(GL_LINE_LOOP, 4, 4); // Parede direita com abertura
  glDrawArrays(GL_LINE_LOOP, 8, 4); // Parede superior
  glDrawArrays(GL_LINE_LOOP, 12,
               6); // Parede inferior com abertura fixa tipo fosso

  glDeleteBuffers(1, &VBO);
}