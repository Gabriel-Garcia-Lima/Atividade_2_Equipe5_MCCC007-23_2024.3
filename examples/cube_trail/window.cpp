#include "window.hpp"

#include <filesystem>

void Window::onEvent(SDL_Event const &event) {
  if (event.type == SDL_KEYDOWN) {
    if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_UP)
      m_cube.moveUp();
    if (event.key.keysym.sym == SDLK_s || event.key.keysym.sym == SDLK_DOWN)
      m_cube.moveDown();
    if (event.key.keysym.sym == SDLK_a || event.key.keysym.sym == SDLK_LEFT)
      m_cube.moveLeft();
    if (event.key.keysym.sym == SDLK_d || event.key.keysym.sym == SDLK_RIGHT)
      m_cube.moveRight();
  }
}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  m_viewMatrix =
      glm::lookAt(glm::vec3(1.9f, 1.9f, 1.9f), glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "cube_trail.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "cube_trail.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  m_viewMatrixLoc = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLoc = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLoc = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLoc = abcg::glGetUniformLocation(m_program, "color");

  m_ground.create(m_program, m_modelMatrixLoc, m_colorLoc, m_scale, m_N);
  m_cube.loadObj(assetsPath + "box.obj");
  m_cube.create(m_program, m_modelMatrixLoc, m_colorLoc, m_viewMatrix, m_scale,
                m_N);

  // Vincula a instância de Ground ao Cube
  m_cube.setGround(&m_ground);
}


void Window::onUpdate() {
  m_cube.update(gsl::narrow_cast<float>(getDeltaTime()));
}

void Window::onPaint() {

  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
  abcg::glUseProgram(m_program);

  // Set uniform variables that have the same value for every model
  auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                    gsl::narrow<float>(m_viewportSize.y)};

  m_projMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);

  abcg::glUniformMatrix4fv(m_viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);

  m_cube.paint();
  m_ground.paint();

  abcg::glUseProgram(0);
}

void Window::onResize(glm::ivec2 const &size) { m_viewportSize = size; }

void Window::onDestroy() {
  m_ground.destroy();
  m_cube.destroy();
  abcg::glDeleteProgram(m_program);
}
