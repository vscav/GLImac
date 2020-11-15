#version 300 es

precision mediump float;

// Attributs de sommet
layout(location = 0) in vec3 aVertexPosition; // Vertice position
layout(location = 1) in vec3 aVertexNormal; // Vertice normal
layout(location = 2) in vec2 aVertexTexCoords; // Vertice texture coordinates

// Transformations matrices
uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;

// Vertex shader outputs
out vec3 vPosition_vs; // Vertice position transformed in View space
out vec3 vNormal_vs; // Vertice normale transformed in View space
out vec2 vTexCoords; // Vertice texture coordinates

void main() {
    // Passage in homogeneous coordinates
    vec4 vertexPosition = vec4(aVertexPosition, 1.0);
    vec4 vertexNormal = vec4(aVertexNormal, 0.0);

    // Calcul of outputs value
    vPosition_vs = vec3(uMVMatrix * vertexPosition);
    vNormal_vs = vec3(uNormalMatrix * vertexNormal);
    vTexCoords = aVertexTexCoords;

    // Calcul of projected position
    gl_Position = uMVPMatrix * vertexPosition;
}