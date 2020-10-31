#version 300 es

precision mediump float;

in vec2 vTextureCoordinates;

out vec2 fFragTextureCoordinates;

void main() {
    fFragTextureCoordinates = vTextureCoordinates;
}