#version 300 es

precision mediump float;

in vec3 vTexCoords;

uniform samplerCube skybox;

out vec4 fFragColor;

void main()
{
    fFragColor = texture(skybox, vTexCoords);
}