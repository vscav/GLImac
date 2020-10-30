#version 300 es

precision mediump float;

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aVertexTextureCoordinates;

// out vec2 vTextureCoordinates;
out vec3 vColor;

uniform mat3 uModelMatrix;
uniform vec3 uColor;

void main() {
    // vTextureCoordinates = aVertexTextureCoordinates;
    // vTextureCoordinates = vec2(1.0, 0.0);
    vColor = uColor;
    vec2 transformed = (uModelMatrix * vec3(aVertexPosition, 1)).xy;
    gl_Position = vec4(transformed, 0, 1);
}