#version 300 es

precision mediump float;

in vec2 vTexCoords;

out vec3 fFragColor;

uniform sampler2D uTexture;

void main() {
    fFragColor = (texture(uTexture, vTexCoords)).xyz;
}