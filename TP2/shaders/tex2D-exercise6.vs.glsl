#version 300 es

precision mediump float;

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec2 aVertexTextureCoordinates;

out vec2 vTextureCoordinates;

uniform mat3 uModelMatrix;

mat3 rotate(float a) {
    float aRad = radians(a);
    mat3 M = mat3(vec3(cos(aRad), sin(aRad), 0), vec3(-sin(aRad), cos(aRad), 0), vec3(0, 0, 1));
    return M;
}

void main() {
    // vTextureCoordinates = aVertexTextureCoordinates;
    vTextureCoordinates = vec2(1.0, 0.0);
    vec2 transformed = (uModelMatrix * vec3(aVertexPosition, 1)).xy;
    gl_Position = vec4(transformed, 0, 1);
}