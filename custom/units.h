#ifndef UNITS_H
#define UNITS_H

#define _FIREBALL_ID 102

#include<engine.h>

Model piramid_ ({
                    -0.866f, 0.05f,-0.5f,
                    0.0f, 2.05f, 0.0f,
                    0.866f, 0.05f,-0.5f,

                    0.866f, 0.05f,-0.5f,
                    0.0f, 2.05f, 0.0f,
                    0.0f, 0.05f, 1.0f,

                    0.0f, 0.05f, 1.0f,
                    0.0f, 2.05f, 0.0f,
                    -0.866f, 0.05f,-0.5f,

                    -0.866f, -0.05f,-0.5f,
                    0.0f, -2.05f, 0.0f,
                    0.866f, -0.05f,-0.5f,

                    0.866f, -0.05f,-0.5f,
                    0.0f, -2.05f, 0.0f,
                    0.0f, -0.05f, 1.0f,

                    0.0f, -0.05f, 1.0f,
                    0.0f, -2.05f, 0.0f,
                    -0.866f, -0.05f,-0.5f
                }, {
                    0.0f,  0.7f,  0.7f,
                    0.0f,  1.0f,  0.7f,
                    0.0f,  0.7f,  1.0f,

                    0.0f,  0.7f,  1.0f,
                    0.0f,  1.0f,  0.7f,
                    0.3f,  0.7f,  0.7f,

                    0.3f,  0.7f,  0.7f,
                    0.0f,  1.0f,  0.7f,
                    0.0f,  0.7f,  0.7f,

                    0.0f,  0.7f,  0.7f,
                    0.1f,  0.5f,  0.5f,
                    0.0f,  0.7f,  1.0f,

                    0.0f,  0.7f,  1.0f,
                    0.1f,  0.5f,  0.5f,
                    0.3f,  0.7f,  0.7f,

                    0.3f,  0.7f,  0.7f,
                    0.1f,  0.5f,  0.5f,
                    0.0f,  0.7f,  0.7f,
                });
class Piramid : public Unit {
public:
    const static int id = 101;
    Piramid(glm::vec3 pos) : Unit(&piramid_, pos){}
    void idle(float dt) {
        float rotation_speed = 1 / 1000000.0;
        _euler += glm::vec3(0, 0, dt*rotation_speed);
    }
    static void load_model() {
        piramid_.load();
    }
    static Model model() {
        return piramid_;
    }
    void on_collision(int id) {
        if (id == _FIREBALL_ID)
            _visible = false;
    }
};




Sphere4Model* sph_;
class Fireball : public Unit {
public:
    const static int id = _FIREBALL_ID;
    Fireball(glm::vec3 pos, glm::vec3 dir_speed, unsigned del_threshold = 10000)
        : Unit(sph_, pos), _dir_speed(dir_speed), _del_threshold(del_threshold) {}
    void fly(float dt) {
        _pos += _dir_speed * dt;
    }
    static void load_model() {
        sph_ = new Sphere4Model();
    }
    static Sphere4Model model() {
        return *sph_;
    }
    bool out_of_map() {
        if (_pos[0]*_pos[0] + _pos[1]*_pos[1] + _pos[2]*_pos[2] > _del_threshold) {
            return true;
        }
        return false;
    }
    void on_collision(int id) {
        if (id == Piramid::id)
            _visible = false;
    }
protected:
    glm::vec3 _dir_speed;
    unsigned _del_threshold;
};


#endif // UNITS_H
