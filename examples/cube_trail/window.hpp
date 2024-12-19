#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcgOpenGL.hpp"
#include "cube.hpp"
#include "ground.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  glm::ivec2 m_viewportSize{600, 600};
  float m_scale{0.2f};
  int m_N{3}; // Número de tiles do chão, 2N+1 x 2N+1

  GLint m_modelMatrixLoc{};
  glm::mat4 m_viewMatrix{1.0f};
  GLint m_viewMatrixLoc{};
  glm::mat4 m_projMatrix{1.0f};
  GLint m_projMatrixLoc{};

  GLint m_colorLoc{};

  Ground m_ground;
  Cube m_cube;
  GLuint m_program{};

  GLuint loadTexture(std::string_view path);
};

#endif
