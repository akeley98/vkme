#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 instancePosition;
layout(location = 3) in vec3 instanceColor;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = vec4(inPosition + instancePosition, 0.0, 1.0);
    fragColor = mix(inColor, instanceColor, 0.5);
}
