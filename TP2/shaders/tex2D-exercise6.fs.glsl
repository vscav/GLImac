#version 300 es

precision mediump float;

uniform vec3 uColor;

out vec3 fFragColor;

void main() {
    fFragColor = uColor;
}