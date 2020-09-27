// Camera class. Mostly just some vectors and matrices and stuff.
#ifndef MYRICUBE_CAMERA_HH_
#define MYRICUBE_CAMERA_HH_

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include "util.hh"
#include "glm/gtc/matrix_transform.hpp"

namespace myricube {

class Camera
{
    // Position of the eye.
    glm::dvec3 eye = glm::dvec3(0, 0, 0);

    // Near and far plane for z-depth.
    // Far plane influences the chunk render distance.
    float near_plane = 0.1f;
    int far_plane = 512;

    // (roughly) minimum distance from the camera that a chunk needs
    // to be to switch from mesh to raycast graphics.
    // Keep as int to avoid rounding errors in distance culling.
    int raycast_threshold = 120;

    // Horizontal and vertical angle camera is pointed in.
    float theta = -1.5707f, phi = 1.5707f;

    // Field of view (y direction), radians.
    float fovy_radians = 1.0f;

    // Window size in pixels.
    int window_x = 1, window_y = 1;

    // Maximum number of new chunk groups added to GPU memory per frame.
    int max_frame_new_chunk_groups = 10;

    // Fog setting.
    bool fog_enabled = true;
    bool black_fog = false;

    // *** True when members below need to be recomputed due to ***
    // *** changes in members above.                            ***
    bool dirty = true;

    // Frenet frame of the camera (unit vectors).
    glm::vec3 forward_normal_vector;
    glm::vec3 right_vector;
    glm::vec3 up_vector;

    glm::mat4 view_matrix;

    // Projection matrix.
    glm::mat4 projection_matrix;

    // Projection * View matrix
    glm::mat4 vp_matrix;

  public:
    // Respond if needed to dirty flag and recompute derived data.
    void fix_dirty()
    {
        if (!dirty) return;

        forward_normal_vector = glm::vec3(
            sinf(phi) * cosf(theta),
            cosf(phi),
            sinf(phi) * sinf(theta));

        right_vector = glm::normalize(
            glm::cross(forward_normal_vector, glm::vec3(0, 1, 0)));
        up_vector = glm::cross(right_vector, forward_normal_vector);

        view_matrix = glm::lookAt(glm::vec3(eye),
                                  glm::vec3(eye) + forward_normal_vector,
                                  // glm::vec3(0, 0, 0),
                                  glm::vec3(0, 1, 0));

        projection_matrix = glm::perspective(
            float(fovy_radians),
            float(window_x) / window_y,
            float(near_plane),
            float(far_plane));

        vp_matrix = projection_matrix * view_matrix;

        dirty = false;
    }

    // Getters and setters for user-specified camera data.
    glm::dvec3 get_eye() const
    {
        return eye;
    }

    float get_near_plane() const
    {
        return near_plane;
    }

    void set_near_plane(float in)
    {
        near_plane = in;
        dirty = true;
    }

    int get_far_plane() const
    {
        return far_plane;
    }

    void set_far_plane(int in)
    {
        far_plane = in;
        dirty = true;
    }

    int get_raycast_threshold() const
    {
        return raycast_threshold;
    }

    void set_raycast_threshold(int in)
    {
        raycast_threshold = in;
        dirty = true;
    }

    void set_eye(glm::dvec3 in)
    {
        assert(is_real(in));
        dirty = true;
        eye = in;
    }

    void inc_eye(glm::dvec3 deye)
    {
        set_eye(eye + deye);
    }

    float get_theta() const
    {
        return theta;
    }

    void set_theta(float in)
    {
        assert(is_real(in));
        dirty = true;
        theta = in;
    }

    void inc_theta(float dtheta)
    {
        set_theta(theta + dtheta);
    }

    float get_phi() const
    {
        return phi;
    }

    void set_phi(float in)
    {
        assert(is_real(in));
        dirty = true;
        phi = glm::clamp(in, 0.01f, 3.14f);
    }

    void inc_phi(float dphi)
    {
        set_phi(phi + dphi);
    }

    float get_fovy_radians() const
    {
        return fovy_radians;
    }

    void set_fovy_radians(float in)
    {
        assert(is_real(in));
        assert(in > 0);
        dirty = true;
        fovy_radians = in;
    }

    void set_window_size(int x, int y)
    {
        dirty = true;
        window_x = x;
        window_y = y;
    }

    bool get_fog() const
    {
        return fog_enabled;
    }

    void set_fog(bool in)
    {
        fog_enabled = in;
    }

    bool use_black_fog() const
    {
        return black_fog;
    }

    bool use_black_fog(bool in)
    {
        return black_fog = in;
    }

    int get_max_frame_new_chunk_groups() const
    {
        return max_frame_new_chunk_groups;
    }

    void set_max_frame_new_chunk_groups(int in)
    {
        max_frame_new_chunk_groups = in;
    }

    // Move by the specified multiples of the normal right, up, and
    // forward vectors respectively.
    void frenet_move(float right, float up, float forward)
    {
        fix_dirty();
        inc_eye(glm::dvec3(
            right * right_vector
          + up * up_vector
          + forward * forward_normal_vector));
    }

    // Getters for derived linear algebra data.
    glm::vec3 get_forward_normal()
    {
        fix_dirty();
        return forward_normal_vector;
    }

    glm::vec3 get_right_normal()
    {
        fix_dirty();
        return right_vector;
    }

    glm::vec3 get_up_normal()
    {
        fix_dirty();
        return up_vector;
    }

    glm::mat4 get_view()
    {
        fix_dirty();
        return view_matrix;
    }

    glm::mat4 get_projection()
    {
        fix_dirty();
        return projection_matrix;
    }

    glm::mat4 get_vp()
    {
        fix_dirty();
        return vp_matrix;
    }
};

} // end namespace
#endif /* !MYRICUBE_CAMERA_HH_ */
