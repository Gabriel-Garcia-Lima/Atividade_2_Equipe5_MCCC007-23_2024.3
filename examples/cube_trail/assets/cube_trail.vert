#version 300 es

precision mediump float;


layout(location = 0) in vec3 inPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 fragV;

void main() {
  vec3 P = (viewMatrix * modelMatrix * vec4(inPosition, 1.0)).xyz;
  
  gl_Position = projMatrix * vec4(P, 1.0);
}
