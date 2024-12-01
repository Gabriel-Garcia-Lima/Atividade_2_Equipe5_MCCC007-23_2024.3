#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include "abcg.hpp"
#include "abcgOpenGL.hpp"
#include "gamedata.hpp"
#include <random>
#include <vector>

class Window final : public abcg::OpenGLWindow {
public:
  ~Window() = default;

  GLuint m_program{};
  GLint m_colorLoc{};
  GLint m_translateLoc{};
  GLint m_scaleLoc{};
  GLint m_rotateLoc{};
  GLuint m_VAO{};

  float m_gameScale{0.15f};
  bool m_gameStarted{false};

  struct Ball {
    glm::vec2 position{};
    glm::vec2 velocity{};
    float radius{};
  };

  Ball m_ball;

private:
  std::vector<Obstacle> m_obstacles;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onDestroy() override;
  void onEvent(SDL_Event const &event) override;

  void setupBall();
  void setupFlippers();
  void checkCollisions();
  void checkWallCollision();
  void checkBottomWallCollision();
  void checkCollisionWithObstacles();
};

#endif
