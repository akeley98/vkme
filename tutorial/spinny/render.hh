#include "camera.hh"
#include "window.hh"

class Renderer;

Renderer* new_renderer(myricube::Window&);
void delete_renderer(Renderer*);
void draw_frame(Renderer*, const myricube::Camera&);

