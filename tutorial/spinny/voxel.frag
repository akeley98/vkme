#version 450
#extension GL_ARB_separate_shader_objects : enable

#define BORDER_WIDTH_LOW  0.075
#define BORDER_WIDTH_HIGH 0.45
#define BORDER_FADE  0.75
#define BORDER_DIST_LOW  100
#define BORDER_DIST_HIGH 350

layout(push_constant) uniform PushConstantBlock {
    mat4 mvp;
    vec4 color;
} PushConstant;

layout(location=0) in vec3 v_color;
layout(location=1) in vec3 v_residue_coord;
layout(location=2) in vec2 v_uv;

layout(location=0) out vec4 out_color;

// vec3 fog_color_from_world_direction(vec3 world_direction);

// vec4 fog_border_color(
//     vec3 base_color,
//     float dist_squared,
//     vec2 uv,
//     vec3 fog_color);

// Utility function for border effect (and setting alpha=1).
vec4 border_color(
    vec3 base_color, // The stored color of the voxel.
    vec2 uv) // "Texture coordinate"
{
    // Border fade diminishes with distance. First, calculate how
    // strong the border fade is (which might not actually matter if
    // this fragment is not on the border).
    // float dist = sqrt(dist_squared);
    float dist = 0; // for now.
    const float slope = (1-BORDER_FADE) / (BORDER_DIST_HIGH - BORDER_DIST_LOW);
    float base_border_fade = clamp(
        BORDER_FADE + (dist - BORDER_DIST_LOW) * slope,
        BORDER_FADE, 1.0);

    // Calculate how close this fragment is to the edge of the voxel.
    // Actual border fade is based on this.
    float u_center_dist = abs(uv.x - 0.5 - floor(uv.x));
    float v_center_dist = abs(uv.y - 0.5 - floor(uv.y));
    float l4 = u_center_dist * u_center_dist * u_center_dist * u_center_dist
             + v_center_dist * v_center_dist * v_center_dist * v_center_dist;
    const float magic_low = (0.5-BORDER_WIDTH_LOW) * (0.5-BORDER_WIDTH_LOW)
                          * (0.5-BORDER_WIDTH_LOW) * (0.5-BORDER_WIDTH_LOW);
    const float magic_high = (0.5-BORDER_WIDTH_HIGH) * (0.5-BORDER_WIDTH_HIGH)
                           * (0.5-BORDER_WIDTH_HIGH) * (0.5-BORDER_WIDTH_HIGH);
    float border_fade = clamp(
        1 + (l4 - magic_high) * (base_border_fade - 1)
        * (1 / (magic_low - magic_high)),
        base_border_fade, 1.0);

    return vec4(border_fade * base_color, 1.0);
}


void main() {
    // vec3 disp = v_residue_coord - eye_relative_group_origin;
    // float dist_squared = dot(disp, disp);
    // vec3 fog_color = fog_color_from_world_direction(disp);
    // out_color = fog_border_color(
    //     v_color, dist_squared, v_uv, fog_color);
    // out_color = vec4(v_color, 1.0);
    out_color = border_color(v_color, v_uv);
}
