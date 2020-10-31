#version 300 es

precision mediump float;

in vec3 vPosition_vs; // Vertice position transformed in View space
in vec3 vNormal_vs; // Vertice normale transformed in View space
in vec2 vTexCoords; // Vertice texture coordinates

out vec3 fFragColor;

void main() {
    fFragColor = normalize(vNormal_vs);
}