#ifndef RENDER_HPP
#define RENDER_HPP

#include "window.hpp"
#include <glm/vec2.hpp>

class Render {
public:
  static void renderBall(Window &window);
  static void renderFlipper(Window &window, Flipper const &flipper,
                            bool isLeft);
  static void renderWalls(Window &window);
  static void renderObstacles(Window &window, glm::vec2 const &position,
                              float radius);
};

#endif // RENDER_HPP
