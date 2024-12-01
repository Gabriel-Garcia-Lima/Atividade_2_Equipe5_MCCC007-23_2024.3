#include "window.hpp"
#include "gamedata.hpp"
#include "render.hpp"
#include <random>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

// Função chamada quando a janela é criada
void Window::onCreate() {
  // Shader de vértice que lida com a posição, translação, escala e rotação dos
  // objetos
  auto const *vertexShader =
      R"gl(
    #version 300 es
    layout(location = 0) in vec2 inPosition;
    uniform vec2 translate;
    uniform float scale;
    uniform float rotate;
    void main() {
      vec2 pos = inPosition;
      // Calcula a rotação usando matriz de rotação 2D
      float sinTheta = sin(rotate);
      float cosTheta = cos(rotate);
      vec2 rotPos = vec2(
        pos.x * cosTheta - pos.y * sinTheta,
        pos.x * sinTheta + pos.y * cosTheta
      );
      // Aplica escala e translação
      vec2 finalPos = rotPos * scale + translate;
      gl_Position = vec4(finalPos, 0, 1);
    }
  )gl";

  // Shader de fragmento que define a cor dos objetos
  auto const *fragmentShader =
      R"gl(
    #version 300 es
    precision mediump float;
    uniform vec4 color;
    out vec4 outColor;
    void main() { outColor = color; }
  )gl";

  // Configuração do gerador de números aleatórios
  std::random_device rd;
  std::mt19937 gen(rd());

  // Distribuições para posições e tamanhos aleatórios dos obstáculos
  std::uniform_real_distribution<float> distPosX(WALL_LEFT + 0.2f,
                                                 WALL_RIGHT - 0.2f);
  std::uniform_real_distribution<float> distPosY(-0.2f, WALL_TOP - 0.2f);
  std::uniform_real_distribution<float> distRadius(0.1f, 0.3f);

  // Criação dos obstáculos com posições e raios aleatórios
  int numObstacles = 6;
  m_obstacles.clear();
  for (int i = 0; i < numObstacles; ++i) {
    glm::vec2 randomPosition(distPosX(gen), distPosY(gen));
    float randomRadius = distRadius(gen);
    m_obstacles.push_back({randomPosition, randomRadius});
  }

  // Cria o programa OpenGL combinando os shaders
  m_program = abcg::createOpenGLProgram(
      {{.source = vertexShader, .stage = abcg::ShaderStage::Vertex},
       {.source = fragmentShader, .stage = abcg::ShaderStage::Fragment}});

  // Obtém as localizações das variáveis uniformes dos shaders
  m_colorLoc = glGetUniformLocation(m_program, "color");
  m_translateLoc = glGetUniformLocation(m_program, "translate");
  m_scaleLoc = glGetUniformLocation(m_program, "scale");
  m_rotateLoc = glGetUniformLocation(m_program, "rotate");

  // Cria e configura o Vertex Array Object
  glGenVertexArrays(1, &m_VAO);
  glBindVertexArray(m_VAO);

  // Configura a bola e os flippers
  setupBall();
  setupFlippers();

  // Define a cor de fundo e a largura das linhas
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glLineWidth(10.0f);
}

// Verifica colisões entre a bola e os obstáculos
void Window::checkCollisionWithObstacles() {
  float scaledBallRadius = m_ball.radius * m_gameScale;

  for (auto const &obstacle : m_obstacles) {
    // Calcula a distância entre o centro da bola e o centro do obstáculo
    float distance = glm::length(m_ball.position - obstacle.position);

    // Verifica se há sobreposição entre a bola e o obstáculo
    if (distance < (scaledBallRadius + obstacle.radius / 2.5)) {
      // Calcula a reflexão da velocidade da bola usando o vetor normal
      glm::vec2 normal = glm::normalize(m_ball.position - obstacle.position);
      m_ball.velocity = glm::reflect(m_ball.velocity, normal);
    }
  }
}

// Configura a posição inicial da bola
void Window::setupBall() {
  m_ball.position = {1.1f, 0.8f}; // Posição inicial fora da área de jogo
  m_ball.velocity = {0.0f, 0.0f}; // Velocidade inicial zero
  m_ball.radius = 0.20f;
}

// Configura a posição e ângulos iniciais dos flippers
void Window::setupFlippers() {
  // Flipper esquerdo
  m_leftFlipper.position = {-0.5f, -0.8f};
  m_leftFlipper.currentAngle = -0.3f;
  m_leftFlipper.targetAngle = -0.3f;
  m_leftFlipper.length = 0.4f;

  // Flipper direito
  m_rightFlipper.position = {0.5f, -0.8f};
  m_rightFlipper.currentAngle = -0.3f;
  m_rightFlipper.targetAngle = -0.3f;
  m_rightFlipper.length = 0.4f;
}

// Atualiza o estado do jogo a cada frame
void Window::onUpdate() {
  if (!m_gameStarted)
    return;

  auto const deltaTime{getDeltaTime()};
  if (deltaTime <= std::numeric_limits<float>::epsilon())
    return;

  // Configurações de velocidade angular dos flippers
  float maxAngularSpeed = 5.0f;
  float maxAngularVelocity = 5.0f;

  // Lambda function para atualizar os ângulos dos flippers
  auto updateFlipperAngle = [&](Flipper &flipper) {
    float previousAngle = flipper.currentAngle;
    float angleDifference = flipper.targetAngle - flipper.currentAngle;
    float angleStep = maxAngularSpeed * deltaTime;

    // Atualiza o ângulo atual
    if (std::abs(angleDifference) < angleStep) {
      flipper.currentAngle = flipper.targetAngle;
    } else {
      flipper.currentAngle += (angleDifference > 0 ? angleStep : -angleStep);
    }

    // Calcula e limita a velocidade angular
    if (deltaTime > std::numeric_limits<float>::epsilon()) {
      flipper.angularVelocity =
          (flipper.currentAngle - previousAngle) / deltaTime;
      flipper.angularVelocity = std::clamp(
          flipper.angularVelocity, -maxAngularVelocity, maxAngularVelocity);
    } else {
      flipper.angularVelocity = 0.0f;
    }
  };

  // Atualiza ambos os flippers
  updateFlipperAngle(m_leftFlipper);
  updateFlipperAngle(m_rightFlipper);

  // Aplica gravidade à bola
  m_ball.velocity.y -= 0.8f * deltaTime;

  // Verifica se a velocidade da bola é válida
  if (glm::any(glm::isnan(m_ball.velocity)) ||
      glm::any(glm::isinf(m_ball.velocity))) {
    m_ball.velocity = glm::vec2(0.0f);
  }

  // Atualiza a posição da bola e verifica colisões
  m_ball.position += m_ball.velocity * static_cast<float>(deltaTime);
  checkCollisions();
  checkCollisionWithObstacles();
}

// Manipula eventos de entrada
void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    // Inicia o jogo com a tecla espaço
    if (event.key.keysym.sym == SDLK_SPACE && !m_gameStarted) {
      m_gameStarted = true;
      m_ball.velocity = {2.0f, 0.5f};
    }
    // Controle dos flippers
    if (event.key.keysym.sym == SDLK_LEFT)
      m_leftFlipper.targetAngle = 0.8f;
    if (event.key.keysym.sym == SDLK_RIGHT)
      m_rightFlipper.targetAngle = 0.8f;
  }
  // Reset dos flippers quando as teclas são soltas
  else if (event.type == SDL_KEYUP) {
    if (event.key.keysym.sym == SDLK_LEFT)
      m_leftFlipper.targetAngle = -0.5f;
    if (event.key.keysym.sym == SDLK_RIGHT)
      m_rightFlipper.targetAngle = -0.5f;
  }
}

// Sistema de colisões
void Window::checkCollisions() {
  const float flipperHalfHeight = 0.025f;

  // Lambda function para verificar colisão entre bola e flipper
  auto checkBallFlipperCollision = [&](Ball &ball, Flipper const &flipper,
                                       bool isLeft) {
    glm::vec2 relPos = ball.position - flipper.position;
    float angle = isLeft ? flipper.currentAngle : -flipper.currentAngle;
    glm::vec2 rotatedPos = glm::rotate(relPos, -angle);

    float xMin = isLeft ? 0.0f : -flipper.length;
    float xMax = isLeft ? flipper.length : 0.0f;

    // Verifica se há colisão
    if (rotatedPos.x >= xMin && rotatedPos.x <= xMax &&
        rotatedPos.y >= -flipperHalfHeight &&
        rotatedPos.y <= flipperHalfHeight) {

      // Calcula a nova velocidade após a colisão
      glm::vec2 normal = glm::rotate(glm::vec2(0.0f, 1.0f), angle);
      glm::vec2 flipperVelocity =
          flipper.angularVelocity * glm::vec2(-relPos.y, relPos.x);

      // Limita a velocidade do flipper
      float maxFlipperSpeed = 2.0f;
      if (glm::length(flipperVelocity) > maxFlipperSpeed) {
        flipperVelocity = glm::normalize(flipperVelocity) * maxFlipperSpeed;
      }

      // Calcula e aplica a nova velocidade da bola
      glm::vec2 relativeVelocity = ball.velocity - flipperVelocity;
      glm::vec2 newVelocity =
          glm::reflect(relativeVelocity, normal) + flipperVelocity;

      if (!glm::any(glm::isnan(newVelocity)) &&
          !glm::any(glm::isinf(newVelocity))) {
        ball.velocity = newVelocity;
      }
    }
  };

  // Verifica colisões com ambos os flippers
  checkBallFlipperCollision(m_ball, m_leftFlipper, true);
  checkBallFlipperCollision(m_ball, m_rightFlipper, false);

  // Verifica colisões com as paredes
  checkWallCollision();
  checkBottomWallCollision();

  // Reset da bola se ela cair muito abaixo
  float scaledBallRadius = m_ball.radius * m_gameScale;
  if ((m_ball.position.y - scaledBallRadius) < WALL_BOTTOM) {
    setupBall();
    m_gameStarted = false;
  }
}

// Verifica colisões com as paredes laterais e superior
void Window::checkWallCollision() {
  float scaledBallRadius = m_ball.radius * m_gameScale;

  // Colisão com parede esquerda
  if ((m_ball.position.x - scaledBallRadius) < (WALL_LEFT + 0.1f)) {
    m_ball.position.x = (WALL_LEFT + 0.1f) + scaledBallRadius;
    m_ball.velocity.x *= -0.8f;
  }

  // Colisão com parede direita
  if ((m_ball.position.x + scaledBallRadius) > (WALL_RIGHT - 0.1f)) {
    m_ball.position.x = (WALL_RIGHT - 0.1f) - scaledBallRadius;
    m_ball.velocity.x *= -0.8f;
  }

  // Colisão com parede superior
  if ((m_ball.position.y + scaledBallRadius) > WALL_TOP) {
    m_ball.position.y = WALL_TOP - scaledBallRadius;
    m_ball.velocity.y *= -0.8f;
  }
}

// Verifica colisões com as paredes inferiores
void Window::checkBottomWallCollision() {
  float scaledBallRadius = m_ball.radius * m_gameScale;

  // Colisão com parede inferior esquerda
  if ((m_ball.position.y - scaledBallRadius) < (WALL_BOTTOM + 0.2f) &&
      (m_ball.position.x - scaledBallRadius) < (m_leftFlipper.position.x)) {
    m_ball.position.y = WALL_BOTTOM + 0.2f + scaledBallRadius;
    m_ball.velocity.y *= -0.8f;
  }

  // Colisão com parede inferior direita
  if ((m_ball.position.y - scaledBallRadius) < (WALL_BOTTOM + 0.2f) &&
      (m_ball.position.x + scaledBallRadius) > (m_rightFlipper.position.x)) {
    m_ball.position.y = WALL_BOTTOM + 0.2f + scaledBallRadius;
    m_ball.velocity.y *= -0.8f;
  }
}

// Renderiza os elementos do jogo
void Window::onPaint() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(m_program);

  glUniform1f(m_scaleLoc, m_gameScale);

  // Renderiza todos os elementos do jogo
  Render::renderWalls(*this);
  Render::renderBall(*this);
  Render::renderFlipper(*this, m_leftFlipper, true);
  Render::renderFlipper(*this, m_rightFlipper, false);

  for (auto const &obstacle : m_obstacles) {
    Render::renderObstacles(*this, obstacle.position, obstacle.radius);
  }

  glBindVertexArray(0);
  glUseProgram(0);
}

void Window::onDestroy() {
  if (m_VAO != 0)
    glDeleteVertexArrays(1, &m_VAO);
  if (m_program != 0)
    glDeleteProgram(m_program);
}
