// camera.hpp

#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  void initialize();
  void updateProjectionMatrix(int width, int height);

  glm::mat4 getViewMatrix() const;
  glm::mat4 getProjectionMatrix(int width, int height) const;

private:
  glm::vec3 m_position;
  glm::vec3 m_target;
  glm::vec3 m_up;
  float m_fov;
  float m_nearPlane;
  float m_farPlane;
  float m_aspectRatio;
};

#endif
