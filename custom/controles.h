#ifndef CONTROLE_H
#define CONTROLE_H

#include<engine.h>
#include<custom/units.h>
#include<set>

void piramid_controller(Loop* loop) {
    static int PiramidNum = 10;
    static Piramid** piramids = new Piramid*[PiramidNum];
    static bool init = true;
    if (init) {
        for (int i=0; i < PiramidNum; i++) {
            float angel = i*2*3.14/PiramidNum;
            piramids[i] = new Piramid(glm::vec3(10.0*cos(angel), 0, 10.0*sin(angel)));
            loop->collision_controller.add_to_group(Piramid::id, piramids[i], glm::vec3(1, 2, 1));
        }
        init = false;
    }

    for (int i=0; i < PiramidNum; i++) {
        piramids[i]->draw(*loop->window());
        piramids[i]->idle(loop->scaled_delta());
    }
}

void movement_controle(Loop* loop) {
    static Window& win = *loop->window();
    static const float camera_speed = 0.00002;
    int dt = loop->real_delata();
    if (win.key_pressed(GLFW_KEY_UP) || win.key_pressed(GLFW_KEY_W)) win.move_forward(dt*camera_speed);
    if (win.key_pressed(GLFW_KEY_DOWN) || win.key_pressed(GLFW_KEY_S)) win.move_forward(-dt*camera_speed);
    if (win.key_pressed(GLFW_KEY_RIGHT) || win.key_pressed(GLFW_KEY_D)) win.move_right(dt*camera_speed);
    if (win.key_pressed(GLFW_KEY_LEFT) || win.key_pressed(GLFW_KEY_A)) win.move_right(-dt*camera_speed);
    if (win.key_pressed(GLFW_KEY_SPACE)) win.move_up(dt*camera_speed);
    if (win.key_pressed(GLFW_KEY_LEFT_SHIFT)) win.move_up(-dt*camera_speed);
    if (win.key_pressed(GLFW_KEY_Q)) win.set_camera(glm::vec3(0, 5, -10), 0, 0);
}


void detalization_controle(Loop* loop) {
    static bool spliting_plus = false;
    static bool spliting_minus = false;

    if (loop->window()->key_pressed(GLFW_KEY_EQUAL) && !spliting_plus) {
        spliting_plus = true;
        sph_->split();
    }
    if (loop->window()->key_released(GLFW_KEY_EQUAL)) spliting_plus = false;
    if (loop->window()->key_pressed(GLFW_KEY_MINUS) && !spliting_plus && !spliting_minus) {
        spliting_minus = true;
        sph_->unsplit();
    }
    if (loop->window()->key_released(GLFW_KEY_MINUS)) spliting_minus = false;
}

void fireball_controle(Loop* loop) {
    static Window& win = *loop->window();
    static std::set<Fireball*> fireball_set;
    static bool fireball_pressed = false;
    if (loop->window()->key_pressed(GLFW_KEY_E) && !fireball_pressed) {
        fireball_pressed = true;
        Fireball* new_fireball = new Fireball(win.get_camera_pos() + win.get_camera_right(), win.get_camera_dir() * 0.0001f);
        loop->collision_controller.add_to_group(Fireball::id, new_fireball, glm::vec3(1, 1, 1));
        fireball_set.insert(new_fireball);
    }
    if (win.key_released(GLFW_KEY_E)) fireball_pressed = false;
    for (auto fireball = fireball_set.begin(); fireball != fireball_set.end();) {
        (*fireball)->draw(win);
        (*fireball)->fly(loop->scaled_delta());
        if ((*fireball)->out_of_map()) {
            fireball_set.erase(fireball++);
            //delete (*fireball);
        }
        else
            ++fireball;
    }
}

void timescale_controle(Loop* loop) {
    static Window& win = *loop->window();
    if (win.key_pressed(GLFW_KEY_0)) loop->time_scale(1.0f);
    if (win.key_pressed(GLFW_KEY_1)) loop->time_scale(0.1f);
    if (win.key_pressed(GLFW_KEY_2)) loop->time_scale(0.2f);
    if (win.key_pressed(GLFW_KEY_3)) loop->time_scale(0.3f);
    if (win.key_pressed(GLFW_KEY_4)) loop->time_scale(0.4f);
    if (win.key_pressed(GLFW_KEY_5)) loop->time_scale(0.5f);
    if (win.key_pressed(GLFW_KEY_6)) loop->time_scale(0.6f);
    if (win.key_pressed(GLFW_KEY_7)) loop->time_scale(0.7f);
    if (win.key_pressed(GLFW_KEY_8)) loop->time_scale(0.8f);
    if (win.key_pressed(GLFW_KEY_9)) loop->time_scale(0.9f);
}

#endif // CONTROLE_H
