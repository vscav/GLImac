#version 300 es

precision mediump float;

out vec4 fFragColor;

in vec3 vNormal_vs;
in vec3 vPosition_vs;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{             
    float ratio = 1.00 / 1.52;
    vec3 I = normalize(Position - cameraPos);
    vec3 R = refract(I, normalize(Normal), ratio);
    fFragColor = vec4(texture(skybox, R).rgb, 1.0);
}