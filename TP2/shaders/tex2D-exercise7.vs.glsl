#version 300 es

precision mediump float;

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aVertexTexture;

out vec2 vFragPosition;
out vec2 vFragTexture;

uniform mat3 uModelMatrix;

void main() {
    vFragPosition = aVertexPosition;
    vFragTexture = aVertexTexture;
    
    vec2 transformed = (uModelMatrix * vec3(aVertexPosition, 1)).xy;
    gl_Position = vec4(transformed, 0, 1);
}