#version 300 es

precision mediump float;

in vec3 vColor;

out vec3 fFragColor;

void main() {
    float bw = (vColor.x + vColor.g + vColor.b) / 3.0;              
    fFragColor = vec3(
        bw,
        bw,
        bw
    );
}