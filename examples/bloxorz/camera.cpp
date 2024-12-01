// camera.cpp

#include "camera.hpp"

void Camera::initialize() {
  m_position = glm::vec3(0.0f, 10.0f, 15.0f);
  m_target = glm::vec3(0.0f, 0.0f, 0.0f);
  m_up = glm::vec3(0.0f, 1.0f, 0.0f);
  m_fov = glm::radians(45.0f);
  m_nearPlane = 0.1f;
  m_farPlane = 100.0f;
  m_aspectRatio = 4.0f / 3.0f; // Valor padrão
}

void Camera::updateProjectionMatrix(int width, int height) {
  m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

glm::mat4 Camera::getViewMatrix() const {
  return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 Camera::getProjectionMatrix(int width, int height) const {
  return glm::perspective(m_fov, m_aspectRatio, m_nearPlane, m_farPlane);
}
