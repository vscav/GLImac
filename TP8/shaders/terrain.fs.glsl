#version 300 es

precision mediump float;

in vec3 vColour_vs;

out vec3 fFragColor;

void main() {
    fFragColor = vColour_vs;
}