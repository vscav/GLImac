#version 300 es

precision mediump float;

layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vColor;

mat3 translate(float tx, float ty) {
    mat3 M = mat3(vec3(1, 0, 0), vec3(0, 1, 0), vec3(tx, ty, 1));
    return M;
}

mat3 scale(float sx, float sy) {
    mat3 M = mat3(vec3(sx, 0, 0), vec3(0, sy, 0), vec3(0, 0, 1));
    return M;
}

mat3 rotate(float a) {
    float aRad = radians(a);
    mat3 M = mat3(vec3(cos(aRad), sin(aRad), 0), vec3(-sin(aRad), cos(aRad), 0), vec3(0, 0, 1));
    return M;
}

void main() {
    vColor = aVertexColor;
    vec2 transformed = (vec3(aVertexPosition, 1)).xy;
    gl_Position = vec4(transformed, 0, 1);
}