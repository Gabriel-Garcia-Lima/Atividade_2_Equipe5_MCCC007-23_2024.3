#version 330 core

out vec4 FragColor;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoord;

uniform vec3 lightPosition;
uniform vec3 objectColor;

void main() {
    // Propriedades da luz
    vec3 lightColor = vec3(1.0f);
    vec3 ambient = 0.2f * lightColor;

    // Iluminação difusa
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPosition - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // Iluminação especular
    vec3 viewDir = normalize(-fragPosition); // Assumindo câmera na origem
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = 0.5f * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0f);
}
