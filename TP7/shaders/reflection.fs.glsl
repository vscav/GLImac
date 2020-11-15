#version 300 es

precision mediump float;

out vec4 fFragColor;

in vec3 vNormal_vs;
in vec3 vPosition_vs;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{             
    vec3 I = normalize(vPosition_vs - cameraPos);
    vec3 R = reflect(I, normalize(vNormal_vs));
    fFragColor = vec4(texture(skybox, R).rgb, 1.0);
}