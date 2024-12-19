#version 300 es

precision mediump float;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

out vec4 outColor;

uniform sampler2D tex;
uniform vec3 lightDir;
uniform vec3 lightColor;
uniform vec3 ambientColor;

void main() {
  // Normaliza a normal
  vec3 N = normalize(fragNormal);
  vec3 L = normalize(-lightDir); // Direção da luz (inversa do lightDir se o lightDir aponta de onde a luz vem)
  
  // Cálculo de Lambert
  float diff = max(dot(N, L), 0.0);

  vec3 color = texture(tex, fragTexCoord).rgb;
  
  vec3 finalColor = ambientColor * color + diff * lightColor * color;
  outColor = vec4(finalColor, 1.0);
}
