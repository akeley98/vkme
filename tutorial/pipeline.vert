#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec3 fragColor;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    vec2 vertex_pos = 0.25 * positions[gl_VertexIndex];
    float angle = 0.7853981633974483 * gl_InstanceIndex;
    vec2 instance_pos = 0.6 * vec2(cos(angle), sin(angle));
    gl_Position = vec4(vertex_pos + instance_pos, 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}
