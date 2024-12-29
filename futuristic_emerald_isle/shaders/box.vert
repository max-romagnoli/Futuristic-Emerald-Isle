#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in vec3 vertexNormal;

out vec3 fragColor;
out vec3 worldPosition;
out vec3 worldNormal;
out vec2 uv;

uniform mat4 MVP;
uniform mat4 modelMatrix;

void main() {
    gl_Position = MVP * vec4(vertexPosition, 1.0);

    // Transform the vertex position to world space
    worldPosition = vec3(modelMatrix * vec4(vertexPosition, 1.0));

    // Transform the normal to world space
    worldNormal = normalize(mat3(transpose(inverse(modelMatrix))) * vertexNormal);

    //fragColor = vertexColor;
    uv = vertexUV;
}
