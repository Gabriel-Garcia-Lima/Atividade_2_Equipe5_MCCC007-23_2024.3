// window.cpp

#include "window.hpp"

#include <imgui.h>

void Window::onCreate() {
  // Configuração inicial do OpenGL
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  // Carrega os shaders
  m_program = createProgramFromFile(getAssetsPath() + "shaders/phong.vert",
                                    getAssetsPath() + "shaders/phong.frag");

  // Inicializa o estado do jogo
  m_level.loadFromFile(getAssetsPath() + "levels/level1.txt");
  m_playerBlock.initialize(m_program, m_level.getStartPosition());
  m_camera.initialize();

  // Configura o tempo
  m_lastFrameTime = static_cast<float>(SDL_GetTicks()) / 1000.0f;
}

void Window::onUpdate() {
  if (m_paused) return;

  // Calcula o deltaTime
  float currentTime = static_cast<float>(SDL_GetTicks()) / 1000.0f;
  m_deltaTime = currentTime - m_lastFrameTime;
  m_lastFrameTime = currentTime;

  // Atualiza a lógica do jogo
  m_playerBlock.update(m_deltaTime, m_level);

  // Verifica condições de vitória ou derrota
  if (m_playerBlock.hasWon(m_level)) {
    // Carrega o próximo nível ou exibe mensagem de vitória
    // Por simplicidade, vamos reiniciar o nível
    m_level.loadFromFile(getAssetsPath() + "levels/level1.txt");
    m_playerBlock.reset(m_level.getStartPosition());
  } else if (m_playerBlock.hasLost(m_level)) {
    // Reinicia o nível
    m_playerBlock.reset(m_level.getStartPosition());
  }
}

void Window::onPaint() {
  // Limpa os buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Define o viewport
  glViewport(0, 0, m_viewportWidth, m_viewportHeight);

  // Ativa o shader program
  glUseProgram(m_program);

  // Configura as matrizes de projeção e visualização
  glm::mat4 viewMatrix = m_camera.getViewMatrix();
  glm::mat4 projectionMatrix = m_camera.getProjectionMatrix(m_viewportWidth, m_viewportHeight);

  GLint viewMatrixLoc = glGetUniformLocation(m_program, "viewMatrix");
  GLint projMatrixLoc = glGetUniformLocation(m_program, "projectionMatrix");

  glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewMatrix[0][0]);
  glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &projectionMatrix[0][0]);

  // Configura a luz
  GLint lightPosLoc = glGetUniformLocation(m_program, "lightPosition");
  glm::vec3 lightPosition = glm::vec3(10.0f, 10.0f, 10.0f);
  glUniform3fv(lightPosLoc, 1, &lightPosition[0]);

  // Renderiza o nível
  m_level.render(m_program);

  // Renderiza o bloco do jogador
  m_playerBlock.render(m_program);

  // Desativa o shader program
  glUseProgram(0);
}

void Window::onPaintUI() {
  // Desenha a interface do usuário com ImGui
  abcg::OpenGLWindow::onPaintUI();

  ImGui::Begin("Informações");
  ImGui::Text("Nível: %d", m_level.getCurrentLevel());
  ImGui::Text("Movimentos: %d", m_playerBlock.getMoveCount());
  ImGui::Checkbox("Pausar", &m_paused);
  ImGui::End();
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportWidth = size.x;
  m_viewportHeight = size.y;

  // Atualiza a projeção da câmera
  m_camera.updateProjectionMatrix(m_viewportWidth, m_viewportHeight);
}

void Window::onDestroy() {
  // Libera recursos alocados
  m_playerBlock.destroy();
  m_level.destroy();

  // Deleta o shader program
  glDeleteProgram(m_program);
}

void Window::onEvent(SDL_Event const &event) {
  // Lida com eventos de entrada do usuário
  if (event.type == SDL_KEYDOWN) {
    switch (event.key.keysym.sym) {
      case SDLK_w:
      case SDLK_UP:
        m_playerBlock.queueMove(Direction::Up);
        break;
      case SDLK_s:
      case SDLK_DOWN:
        m_playerBlock.queueMove(Direction::Down);
        break;
      case SDLK_a:
      case SDLK_LEFT:
        m_playerBlock.queueMove(Direction::Left);
        break;
      case SDLK_d:
      case SDLK_RIGHT:
        m_playerBlock.queueMove(Direction::Right);
        break;
      case SDLK_SPACE:
        m_paused = !m_paused;
        break;
      case SDLK_ESCAPE:
        // Implementar funcionalidade de pausa ou saída
        break;
      default:
        break;
    }
  }
}
