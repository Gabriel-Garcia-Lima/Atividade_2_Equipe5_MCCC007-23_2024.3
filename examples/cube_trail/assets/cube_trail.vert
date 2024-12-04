#version 300 es

precision mediump float;


layout(location = 0) in vec3 inPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 fragV;

void main() {
  gl_Position = projMatrix * viewMatrix * modelMatrix * vec4(inPosition, 1.0);
}
