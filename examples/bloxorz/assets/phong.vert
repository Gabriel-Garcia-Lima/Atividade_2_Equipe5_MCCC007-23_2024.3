#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoord;

void main() {
    fragPosition = vec3(modelMatrix * vec4(aPosition, 1.0));
    fragNormal = mat3(transpose(inverse(modelMatrix))) * aNormal;
    fragTexCoord = aTexCoord;
    gl_Position = projectionMatrix * viewMatrix * vec4(fragPosition, 1.0);
}
