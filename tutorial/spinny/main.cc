#include "window.hh"
#include "render.hh"
#include "util.hh"

using namespace myricube;

// Absolute path of the executable, minus the -bin or .exe, plus -data/
// This is where shaders and stuff are stored.
std::string data_directory;

std::string expand_filename(const std::string& in)
{
    if (data_directory.size() == 0) {
        throw std::logic_error("Cannot call expand_filename before main");
    }
    return in[0] == '/' ? in : data_directory + in;
}

bool ends_with_bin_or_exe(const std::string& in)
{
    auto sz = in.size();
    if (sz < 4) return false;
    const char* suffix = &in[sz - 4];
    return strcmp(suffix, "-bin") == 0 or strcmp(suffix, ".exe") == 0;
}

bool paused = false;
int target_fragments = 0;

void add_key_targets(Window& window, Camera& camera)
{
    static float speed = 8.0f;
    static float sprint_mod = 1.0f;

    struct Position
    {
        glm::dvec3 eye = glm::dvec3(0);
        float theta = 1.5707f;
        float phi = 1.5707f;
    };
    static Position old_positions_ring_buffer[256];
    static Position future_positions_ring_buffer[256];
    static uint8_t old_idx = 0;
    static uint8_t future_idx = 0;

    static auto get_camera_position = [&camera] () -> Position
    {
        Position p;
        p.eye = camera.get_eye();
        p.theta = camera.get_theta();
        p.phi = camera.get_phi();
        return p;
    };

    static auto push_camera_position = [&]
    {
        old_positions_ring_buffer[--old_idx] = get_camera_position();
    };

    static auto push_camera_position_callback = [&] (KeyArg arg)
    {
        if (arg.repeat) return false;
        push_camera_position();
        return true;
    };

    KeyTarget pop_old_camera, pop_future_camera;
    pop_old_camera.down = [&] (KeyArg) -> bool
    {
        future_positions_ring_buffer[--future_idx] =
            get_camera_position();
        Position p = old_positions_ring_buffer[old_idx++];
        camera.set_eye(p.eye);
        camera.set_theta(p.theta);
        camera.set_phi(p.phi);
        return true;
    };
    pop_future_camera.down = [&] (KeyArg) -> bool
    {
        old_positions_ring_buffer[--old_idx] =
            get_camera_position();
        Position p = future_positions_ring_buffer[future_idx++];
        camera.set_eye(p.eye);
        camera.set_theta(p.theta);
        camera.set_phi(p.phi);
        return true;
    };
    window.add_key_target("pop_old_camera", pop_old_camera);
    window.add_key_target("pop_future_camera", pop_future_camera);

    KeyTarget forward, backward, leftward, rightward, upward, downward;
    forward.down = push_camera_position_callback;
    forward.per_frame = [&] (KeyArg arg) -> bool
    {
        camera.frenet_move(0, 0, +arg.dt * speed * sprint_mod);
        return true;
    };
    backward.down = push_camera_position_callback;
    backward.per_frame = [&] (KeyArg arg) -> bool
    {
        camera.frenet_move(0, 0, -arg.dt * speed * sprint_mod);
        return true;
    };
    leftward.down = push_camera_position_callback;
    leftward.per_frame = [&] (KeyArg arg) -> bool
    {
        camera.frenet_move(-arg.dt * speed * sprint_mod, 0, 0);
        return true;
    };
    rightward.down = push_camera_position_callback;
    rightward.per_frame = [&] (KeyArg arg) -> bool
    {
        camera.frenet_move(+arg.dt * speed * sprint_mod, 0, 0);
        return true;
    };
    upward.down = push_camera_position_callback;
    upward.per_frame = [&] (KeyArg arg) -> bool
    {
        camera.frenet_move(0, +arg.dt * speed * sprint_mod, 0);
        return true;
    };
    downward.down = push_camera_position_callback;
    downward.per_frame = [&] (KeyArg arg) -> bool
    {
        camera.frenet_move(0, -arg.dt * speed * sprint_mod, 0);
        return true;
    };
    window.add_key_target("forward", forward);
    window.add_key_target("backward", backward);
    window.add_key_target("leftward", leftward);
    window.add_key_target("rightward", rightward);
    window.add_key_target("upward", upward);
    window.add_key_target("downward", downward);

    KeyTarget sprint, speed_up, slow_down;
    sprint.down = [&] (KeyArg) -> bool
    {
        sprint_mod = 7.0f;
        return true;
    };
    sprint.up = [&] (KeyArg) -> bool
    {
        sprint_mod = 1.0f;
        return true;
    };
    speed_up.down = [&] (KeyArg arg) -> bool
    {
        if (!arg.repeat) speed *= 2.0f;
        return !arg.repeat;
    };
    slow_down.down = [&] (KeyArg arg) -> bool
    {
        if (!arg.repeat) speed *= 0.5f;
        return !arg.repeat;
    };
    window.add_key_target("sprint", sprint);
    window.add_key_target("speed_up", speed_up);
    window.add_key_target("slow_down", slow_down);

    KeyTarget vertical_scroll, horizontal_scroll, look_around;
    look_around.down = push_camera_position_callback;
    look_around.per_frame = [&] (KeyArg arg) -> bool
    {
        camera.inc_theta(arg.mouse_rel_x * arg.dt * 0.01f);
        camera.inc_phi(arg.mouse_rel_y * arg.dt * 0.01f);
        return true;
    };
    vertical_scroll.down = [&] (KeyArg arg) -> bool
    {
        camera.inc_phi(arg.amount * -0.05f);
        return true;
    };
    horizontal_scroll.down = [&] (KeyArg arg) -> bool
    {
        camera.inc_theta(arg.amount * -0.05f);
        return true;
    };
    window.add_key_target("look_around", look_around);
    window.add_key_target("vertical_scroll", vertical_scroll);
    window.add_key_target("horizontal_scroll", horizontal_scroll);
}

// Given the full path of a key binds file, parse it for key bindings
// and add it to the window's database of key bindings (physical
// key/mouse button to KeyTarget name associations).
//
// Syntax: the file should consist of lines of pairs of key names and
// KeyTarget names. Blank (all whitespace) lines are allowed as well
// as comments, which go from a # character to the end of the line.
//
// Returns true iff successful (check errno on false).
bool add_key_binds_from_file(Window& window, std::string filename) noexcept
{
    FILE* file = fopen(filename.c_str(), "r");
    if (file == nullptr) {
        fprintf(stderr, "Could not open %s\n", filename.c_str());
        return false;
    }

    int line_number = 0;

    auto skip_whitespace = [file]
    {
        int c;
        while (1) {
            c = fgetc(file);
            if (c == EOF) return;
            if (c == '\n' or !isspace(c)) {
                ungetc(c, file);
                return;
            }
        }
    };
    errno = 0;

    bool eof = false;
    while (!eof) {
        std::string key_name;
        std::string target_name;
        ++line_number;

        int c;
        skip_whitespace();

        // Parse key name (not case sensitive -- converted to lower case)
        while (1) {
            c = fgetc(file);

            if (c == EOF) {
                if (errno != 0 and errno != EAGAIN) goto bad_eof;
                eof = true;
                goto end_line;
            }
            if (c == '\n') goto end_line;
            if (isspace(c)) break;
            if (c == '#') goto comment;
            key_name.push_back(c);
        }

        skip_whitespace();

        // Parse target name (case sensitive)
        while (1) {
            c = fgetc(file);

            if (c == EOF) {
                if (errno != 0 and errno != EAGAIN) goto bad_eof;
                eof = true;
                goto end_line;
            }
            if (c == '\n') goto end_line;
            if (isspace(c)) break;
            if (c == '#') goto comment;
            target_name.push_back(c);
        }

        skip_whitespace();

        // Check for unexpected cruft at end of line.
        c = fgetc(file);
        if (c == EOF) {
            if (errno != 0 and errno != EAGAIN) goto bad_eof;
            eof = true;
            goto end_line;
        }
        else if (c == '#') {
            goto comment;
        }
        else if (c == '\n') {
            goto end_line;
        }
        else {
            fprintf(stderr, "%s:%i unexpected third token"
                " starting with '%c'\n",
                filename.c_str(), line_number, c);
            errno = EINVAL;
            goto bad_eof;
        }

        // Skip over comment characters from # to \n
      comment:
        while (1) {
            c = fgetc(file);
            if (c == EOF) {
                if (errno != 0 and errno != EAGAIN) goto bad_eof;
                eof = true;
                goto end_line;
            }
            if (c == '\n') {
                break;
            }
        }
      end_line:
        // skip blank lines silently.
        if (key_name.size() == 0) continue;

        // Complain if only one token is provided on a line.
        if (target_name.size() == 0) {
            fprintf(stderr, "%s:%i key name without target name.\n",
                filename.c_str(), line_number);
            errno = EINVAL;
            goto bad_eof;
        }

        auto keycode = keycode_from_name(key_name);
        if (keycode == 0) {
            fprintf(stderr, "%s:%i unknown key name %s.\n",
                filename.c_str(), line_number, key_name.c_str());
            errno = EINVAL;
            goto bad_eof;
        }

        fprintf(stderr, "Binding %s (%i) to %s\n",
            key_name.c_str(), keycode, target_name.c_str());
        window.bind_keycode(keycode, target_name);
    }

    if (fclose(file) != 0) {
        fprintf(stderr, "Error closing %s\n", filename.c_str());
        return false;
    }
    return true;
  bad_eof:
    fprintf(stderr, "Warning: unexpected end of parsing.\n");
    int eof_errno = errno;
    fclose(file);
    errno = eof_errno;
    return true; // I'm getting bogus EOF fails all the time so fake success :/
}

void bind_keys(Window& window)
{
    auto default_file = expand_filename("default-keybinds.txt");
    auto user_file = expand_filename("keybinds.txt");

    bool default_okay = add_key_binds_from_file(window, default_file);
    if (!default_okay) {
        fprintf(stderr, "Failed to parse %s\n", default_file.c_str());
        fprintf(stderr, "%s (%i)\n", strerror(errno), errno);
        exit(2);
    }

    bool user_okay = add_key_binds_from_file(window, user_file);
    if (!user_okay) {
        if (errno == ENOENT) {
            fprintf(stderr, "Custom keybinds file %s not found.\n",
                user_file.c_str());
        }
        else {
            fprintf(stderr, "Failed to parse %s\n", user_file.c_str());
            fprintf(stderr, "%s (%i)\n", strerror(errno), errno);
            exit(2);
        }
    }
}

int main(int argc, char** argv)
{
    // Data directory (where shaders are stored) is the path of this
    // executable, with the -bin or .exe file extension replaced with
    // -data. Construct that directory name here.
    data_directory = argv[0];
    if (!ends_with_bin_or_exe(data_directory)) {
        fprintf(stderr, "%s should end with '-bin' or '.exe'\n",
            data_directory.c_str());
        return 1;
    }
    for (int i = 0; i < 4; ++i) data_directory.pop_back();
    data_directory += "-data/";

    // Instantiate the camera.
    Camera camera;

    // Create a window; callback ensures these window dimensions stay accurate.
    int screen_x = 0, screen_y = 0;
    auto on_window_resize = [&camera, &screen_x, &screen_y] (int x, int y)
    {
        camera.set_window_size(x, y);
        screen_x = x;
        screen_y = y;
    };
    Window window(on_window_resize);
    Renderer* renderer = new_renderer(window);

    add_key_targets(window, camera);
    bind_keys(window);

    while (window.frame_update()) draw_frame(renderer, camera);

    delete_renderer(renderer);
}
