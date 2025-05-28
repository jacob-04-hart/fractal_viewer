#version 400 core
in vec2 uv;
 
out vec4 FragColor;

uniform int maxItr;														
uniform double zoom;																								
uniform dvec2 offset;																						

double get_iterations() {
    double real = (uv.x - 0.5) * zoom + offset.x;
    double imag = (uv.y - 0.5) * zoom + offset.y;

    int iterations = 0;

    double constReal = real;
    double constImag = imag;

    while (iterations < maxItr) {
        double tempReal = real;
        real = (real * real - imag * imag) + constReal;
        imag = (2.0 * tempReal * imag) + constImag;

        if ((real * real + imag * imag) > 4) {
            break;
        }
        iterations++;
    }
    return double(iterations);
}

vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec4 mapColor(double t) {
    float hue = float(0.95 + 10.0 * t); // 0.0-1.0, adjust multiplier for more cycles
    hue = fract(hue);
    float sat = 0.6;
    float val = 1.0;
    vec3 rgb = hsv2rgb(vec3(hue, sat, val));
    return vec4(rgb, 1.0);
}

void main() {
    double iterations = get_iterations();

    if (iterations >= maxItr) {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    } else {
        double t = iterations / double(maxItr);
        FragColor = mapColor(t);
    }
}