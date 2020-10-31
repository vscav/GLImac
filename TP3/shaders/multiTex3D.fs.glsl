#version 300 es

precision mediump float;

in vec2 vTexCoords;

out vec3 fFragColor;

uniform sampler2D uTexture;
uniform sampler2D uSecondTexture;

void main() {
    fFragColor = fFragColor = (texture(uTexture, vTexCoords)).xyz + (texture(uSecondTexture, vTexCoords)).xyz;
}