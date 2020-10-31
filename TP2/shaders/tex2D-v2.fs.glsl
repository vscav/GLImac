#version 300 es

precision mediump float;

in vec2 vFragPosition;
in vec2 vTextureCoordinates;

out vec3 fFragColor;

uniform sampler2D uTexture;

void main() {
    fFragColor = texture(uTexture, vTextureCoordinates).xyz;
}