#include <engine.h>
#include <custom/controles.h>
#include <custom/units.h>

int main() {
    Window win = Window("ogl-staff", 950, 1000);
    win.set_camera(glm::vec3(10, 5, 10), 0, 0);

    Piramid::load_model();
    Fireball::load_model();

    Loop loop = Loop(&win);
    loop.show_fps(false);
    loop.handle_collisions(true);
    loop.collision_controller.create_group(Piramid::id, {Fireball::id});
    loop.collision_controller.create_group(Fireball::id, {Piramid::id});


    Unit sphere = Unit(sph_, glm::vec3(0, 5, 0));
    loop.add_func([&](Loop* loop) {
        sphere.draw(*loop->window());
    });


    loop.add_func(piramid_controller);
    loop.add_func(fireball_controle);
    loop.add_func(movement_controle);
    loop.add_func(detalization_controle);
    loop.add_func(timescale_controle);

    loop.run(60);

    return 0;
}




