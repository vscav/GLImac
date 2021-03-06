#version 300 es

precision mediump float;

in vec2 vFragPosition;
in vec2 vFragTexture;

out vec3 fFragColor;

uniform sampler2D uTexture;

void main() {
    fFragColor = (texture(uTexture, vFragTexture)).xyz;
}