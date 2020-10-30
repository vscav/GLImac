#version 300 es

precision mediump float;

// in vec2 vTextureCoordinates;

// out vec2 fFragTextureCoordinates;

in vec3 vColor;

out vec3 fFragColor;

void main() {
    // fFragTextureCoordinates = vTextureCoordinates;
    fFragColor = vColor;
}