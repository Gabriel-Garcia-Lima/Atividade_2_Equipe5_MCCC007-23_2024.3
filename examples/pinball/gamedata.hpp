#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <glm/vec2.hpp>
#include <vector>

constexpr float WALL_LEFT = -1.0f;
constexpr float WALL_RIGHT = 1.0f;
constexpr float WALL_BOTTOM = -1.0f;
constexpr float WALL_TOP = 1.0f;

struct Flipper {
  glm::vec2 position{};
  float currentAngle{};
  float targetAngle{};
  float angularVelocity{};
  float length{};
};

struct Obstacle {
  glm::vec2 position;
  float radius;
};

extern Flipper m_leftFlipper;
extern Flipper m_rightFlipper;
extern std::vector<Obstacle> m_obstacles;

#endif
