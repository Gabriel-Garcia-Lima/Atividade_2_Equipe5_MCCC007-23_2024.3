// window.hpp

#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcg.hpp"
#include "camera.hpp"
#include "level.hpp"
#include "playerblock.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;
  void onEvent(SDL_Event const &event) override;

private:
  // Gerenciamento do estado do jogo
  PlayerBlock m_playerBlock;
  Level m_level;
  Camera m_camera;

  // Shader program
  GLuint m_program{};

  // Variáveis de projeção e visualização
  int m_viewportWidth{};
  int m_viewportHeight{};

  // Tempo para animações
  float m_deltaTime{};
  float m_lastFrameTime{};

  // Controle de pausa
  bool m_paused{false};
};

#endif
