#version 300 es

precision mediump float;

layout (location = 0) in vec3 aVertexPosition;
out vec3 vTexCoords;

uniform mat4 MVPMatrix;
uniform mat4 MVMatrix;

void main()
{
    gl_Position = MVPMatrix * MVMatrix * vec4(aVertexPosition, 1.0);
    vTexCoords = aVertexPosition;
}