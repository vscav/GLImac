#version 300 es

precision mediump float;

in vec2 vFragPosition;
in vec3 vFragColor;

out vec2 fFragPosition;
out vec3 fFragColor;

void main() {
    float x = 0.0;
    float y = 0.0;
    float d;
    vec2 z;

    int MAX_N = 1000;
    int n = 0;
    do {
        z.x = x * x - y * y + vFragPosition.x;
        z.y = 2.0 * x * y + vFragPosition.y;
        d = sqrt(z.x * z.x + z.y * z.y);
        if(d > 2.0) {
            break;
        }
        x = z.x;
        y = z.y;
        n++;
    } while (n < MAX_N);
    
    if(d > 2.0) {
        fFragColor = vFragColor;
    } else {
        fFragColor = vec3(0.0);
    }
}