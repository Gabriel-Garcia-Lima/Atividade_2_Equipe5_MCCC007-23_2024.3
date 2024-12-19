#include "window.hpp"

#include <fmt/core.h>
#include <tiny_obj_loader.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>

#include "abcg.hpp"
#include "abcgOpenGLImage.hpp" // Necessário para abcg::loadOpenGLTexture
#include "cube.hpp"
#include "ground.hpp"

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

GLuint Window::loadTexture(std::string_view path) {
  // Cria a estrutura de criação da textura
  abcg::OpenGLTextureCreateInfo createInfo{};
  createInfo.path = path;

  // Ajuste de parâmetros da textura (opcional)
  // createInfo.magFilter = GL_LINEAR;
  // createInfo.minFilter = GL_LINEAR_MIPMAP_LINEAR;
  // createInfo.wrapS = GL_REPEAT;
  // createInfo.wrapT = GL_REPEAT;

  // Carrega a textura e retorna o ID
  return abcg::loadOpenGLTexture(createInfo);
}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  abcg::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  abcg::glEnable(GL_DEPTH_TEST);

  m_viewMatrix = glm::lookAt(
      glm::vec3(1.9f, 1.9f, 1.9f), 
      glm::vec3(0.0f, 0.0f, 0.0f), 
      glm::vec3(0.0f, 1.0f, 0.0f));

  // Cria o programa de shader
  m_program = abcg::createOpenGLProgram({
    {.source = assetsPath + "texture_light.vert", .stage = abcg::ShaderStage::Vertex},
    {.source = assetsPath + "texture_light.frag", .stage = abcg::ShaderStage::Fragment}
  });

  m_viewMatrixLoc = abcg::glGetUniformLocation(m_program, "viewMatrix");
  m_projMatrixLoc = abcg::glGetUniformLocation(m_program, "projMatrix");
  m_modelMatrixLoc = abcg::glGetUniformLocation(m_program, "modelMatrix");
  m_colorLoc       = abcg::glGetUniformLocation(m_program, "color");

  // Carrega as texturas para o chão e para o cubo
  auto groundTexture = loadTexture(assetsPath + "tileTexture03.jpg");
  auto cubeTexture   = loadTexture(assetsPath + "cubeTexture03.jpg");

  // Cria o chão e o cubo
  m_ground.create(m_program, m_modelMatrixLoc, m_colorLoc, m_scale, m_N);
  m_ground.setTexture(groundTexture);

  m_cube.loadObj(assetsPath + "box.obj");
  m_cube.create(m_program, m_modelMatrixLoc, m_colorLoc, m_viewMatrix, m_scale, m_N);
  m_cube.setTexture(cubeTexture);

  // Vincula o chão ao cubo
  m_cube.setGround(&m_ground);
}

void Window::onUpdate() {
  m_cube.update(gsl::narrow_cast<float>(getDeltaTime()));
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
  abcg::glUseProgram(m_program);

  auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                    gsl::narrow<float>(m_viewportSize.y)};

  m_projMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);

  abcg::glUniformMatrix4fv(m_viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(m_projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);

  // Define iluminação
  GLint lightDirLoc     = abcg::glGetUniformLocation(m_program, "lightDir");
  GLint lightColorLoc   = abcg::glGetUniformLocation(m_program, "lightColor");
  GLint ambientColorLoc = abcg::glGetUniformLocation(m_program, "ambientColor");

  // Luz vindo de cima, inclinado
  glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
  abcg::glUniform3fv(lightDirLoc, 1, &lightDir.x);

  glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // Cor da Luz ambiente
  abcg::glUniform3fv(lightColorLoc, 1, &lightColor.x);

  glm::vec3 ambientColor = glm::vec3(0.6f, 0.6f, 0.6f); // Iluminação ambiente
  abcg::glUniform3fv(ambientColorLoc, 1, &ambientColor.x);

  // Uniform da textura (sampler2D) é geralmente a unidade 0
  GLint texLoc = abcg::glGetUniformLocation(m_program, "tex");
  abcg::glUniform1i(texLoc, 0);

  m_cube.paint();
  m_ground.paint();

  abcg::glUseProgram(0);
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
}

void Window::onDestroy() {
  m_ground.destroy();
  m_cube.destroy();
  abcg::glDeleteProgram(m_program);
}
