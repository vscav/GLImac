#version 300 es

precision mediump float;

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aVertexTextureCoordinates;

out vec2 vFragPosition;
out vec2 vTextureCoordinates;

uniform mat3 uModelMatrix;

void main() {
    vFragPosition = aVertexPosition;
    vTextureCoordinates = aVertexTextureCoordinates;
    
    vec2 transformed = (uModelMatrix * vec3(aVertexPosition, 1)).xy;
    gl_Position = vec4(transformed, 0, 1);
}