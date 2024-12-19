#version 300 es

precision mediump float;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main() {
  vec4 worldPosition = modelMatrix * vec4(inPosition, 1.0);
  fragPosition = vec3(worldPosition);
  fragNormal = mat3(transpose(inverse(modelMatrix))) * inNormal;
  fragTexCoord = inTexCoord;

  gl_Position = projMatrix * viewMatrix * worldPosition;
}
