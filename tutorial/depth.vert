#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform PushConstantBlock {
    mat4 mvp;
    vec4 color;
} PushConstant;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = PushConstant.mvp * vec4(inPosition, 1.0);
    fragColor = PushConstant.color.rgb;
    fragTexCoord = inTexCoord;
}

