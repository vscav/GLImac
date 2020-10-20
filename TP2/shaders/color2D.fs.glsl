#version 300 es

precision mediump float;

in vec3 vColor;
in vec2 vPosition;

out vec3 fFragColor;

void main() {
    vec2 P = vPosition;

    float distance = distance(vec2(0.0, -0.2), P);
    float mitigation = 0.75 * exp(-20.0 * pow(distance, 2.0));

    fFragColor = vColor * mitigation;

    fFragColor = vColor * smoothstep(0.4, 0.5, max(abs(fract(8.0 * P.x - 0.5 * mod(floor(8.0 * P.y), 2.0)) - 0.5), abs(fract(8.0 * P.y) - 0.5)));

    fFragColor = vColor * mod(floor(10.0 * P.x) + floor(10.0 * P.y), 2.0);

}