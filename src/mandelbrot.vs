#version 330 core
layout(location = 0) in vec3 aPos;
out vec2 uv;
void main() {
    gl_Position = vec4(aPos, 1.0);
    uv = aPos.xy * 0.5 + 0.5; // Map from [-1,1] to [0,1]
}