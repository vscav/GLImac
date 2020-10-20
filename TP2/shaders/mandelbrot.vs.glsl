#version 300 es

precision mediump float;

layout(location = 0) in vec2 aVertexPosition;

out vec2 vFragPosition;
out vec3 vFragColor;

void main() {
    vFragPosition = aVertexPosition;
    vFragColor = vec3(1.0, 1.0, 1.0);
    
    gl_Position = vec4(aVertexPosition, 0, 1);
}