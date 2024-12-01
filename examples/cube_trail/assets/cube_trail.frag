#version 300 es

precision mediump float;

uniform vec4 color;
uniform mat4 viewMatrix;
uniform samplerCube cubeTex;

in vec3 fragV;
out vec4 outColor;

void main() {
  if (gl_FrontFacing) {
    outColor = color;
  } else {
    float i = (color.r + color.g + color.b) / 3.0;
    outColor = vec4(i, 0, 0, 1.0);
  }
}
