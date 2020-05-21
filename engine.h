#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <cmath>
#include <ctime>

#include <vector>
#include <map>
#include <set>

#include <string>
#include <functional>
#include <chrono>
#include <unistd.h>


#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>
#include <common/texture.hpp>

#include<glm/gtx/euler_angles.hpp>


class virtModel {
public:
    virtual void draw() {}
    virtual ~virtModel() {}
};


class Model : public virtModel {
public:
    Model(std::initializer_list<GLfloat> vertexs,
          std::initializer_list<GLfloat> colors)
        : _vertexs(vertexs), _colors(colors) {}
    void load() {
        if (_loaded) return;
        glGenBuffers(1, &_vbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _vbuffer);
        glBufferData(GL_ARRAY_BUFFER, _vertexs.size()*sizeof(GLfloat), _vertexs.data(), GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        glGenBuffers(1, &_cbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _cbuffer);
        glBufferData(GL_ARRAY_BUFFER, _vertexs.size()*sizeof(GLfloat), _colors.data(), GL_STATIC_DRAW);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
        _loaded = true;
    }

    void draw() {
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, _vbuffer);
        glVertexAttribPointer(
                    0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );

        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, _cbuffer);
        glVertexAttribPointer(
                    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                    3,                                // size
                    GL_FLOAT,                         // type
                    GL_FALSE,                         // normalized?
                    0,                                // stride
                    (void*)0                          // array buffer offset
                    );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, 6*3); // 12*3 indices starting at 0 -> 12 triangles

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    }
private:
    std::vector<GLfloat> _vertexs;
    std::vector<GLfloat> _colors;
    GLuint _vbuffer;
    GLuint _cbuffer;
    bool _loaded = false;
};

class Unit;
class Window { //todo: singletone patern or move init to other instance
public:
    friend Unit;
    Window(std::string title, int width = 1024, int height = 768, float FoV = 45.0f):
        _width(width), _height(height) {
        init_gl(title,  width, height);
        // Dark blue background
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
        load_shaders();
        _projection = glm::perspective(glm::radians(FoV), (float) width / height, 0.1f, 100.0f);
    }
    bool exit_check() {
        return glfwGetKey(_window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
                glfwWindowShouldClose(_window) == 0;
    }
    void start_frame() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        double xpos, ypos;
        glfwGetCursorPos(_window, &xpos, &ypos);
        glfwSetCursorPos(_window, _width/2, _height/2);

        _hangel += 0.01 * float(_width/2 - xpos ); // * deltaTime
        _vangel += 0.01 * float(_height/2- ypos );

        if (_vangel < -1.57) _vangel = -1.57;
        if (_vangel > 1.57) _vangel = 1.57;

        set_camera(_pos, _hangel, _vangel);

    }
    void end_frame() {
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }

    bool key_pressed(int key_code) {return glfwGetKey(_window, key_code) == GLFW_PRESS;}
    bool key_released(int key_code) {return glfwGetKey(_window, key_code) == GLFW_RELEASE;}

    void set_camera(glm::vec3 pos, float horizontalAngle, float verticalAngle) {
        _pos = pos;
        _hangel = horizontalAngle;
        _vangel = verticalAngle;

        _dir = glm::vec3(
                    cos(verticalAngle) * sin(horizontalAngle),
                    sin(verticalAngle),
                    cos(verticalAngle) * cos(horizontalAngle)
                    );
        _right = glm::vec3(
                    sin(horizontalAngle - 3.14f/2.0f),
                    0,
                    cos(horizontalAngle - 3.14f/2.0f)
                    );
        glm::mat4 View = glm::lookAt(_pos, _pos + _dir, glm::vec3(0,1,0));
        _vp = _projection * View;
    }

    glm::vec3 get_camera_pos() {return _pos;}
    glm::vec3 get_camera_dir() {return _dir;}
    glm::vec3 get_camera_right() {return _right;}

    void move_forward(float distance) {_pos += _dir * distance;}
    void move_right(float distance) {_pos += _right * distance;}
    void move_up(float distance) {_pos += glm::vec3(0, 1, 0) * distance;}

private:
    void load_shaders() {
        _program_id = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );
        _matrix_location = glGetUniformLocation(_program_id, "MVP");

    }
    void init_gl(std::string title, int width, int height) {

        // Initialise GLFW
        if( !glfwInit() )
        {
            fprintf( stderr, "Failed to initialize GLFW\n" );
            getchar();
            exit(-1);
        }

        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Open a window and create its OpenGL context
        _window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if( _window == NULL ){
            fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
            getchar();
            glfwTerminate();
            exit(-1);
        }
        glfwMakeContextCurrent(_window);

        // Initialize GLEW
        glewExperimental = true; // Needed for core profile
        if (glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to initialize GLEW\n");
            getchar();
            glfwTerminate();
            exit(-1);
        }

        // Ensure we can capture the escape key being pressed below
        glfwSetInputMode(_window, GLFW_STICKY_KEYS, GL_TRUE);
        // Enable depth test
        glEnable(GL_DEPTH_TEST);
        // Accept fragment if it closer to the camera than the former one
        glDepthFunc(GL_LESS);

        GLuint VertexArrayID;
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);
    }
    glm::mat4 _projection;
    glm::mat4 _vp;
    GLuint _program_id;
    GLuint _matrix_location;
    GLFWwindow* _window;

    int _width, _height;
    float _hangel, _vangel;
    glm::vec3 _pos, _dir, _right;
};

class Unit {
public:
    Unit(virtModel* model, glm::vec3 pos, glm::vec3 euler = glm::vec3(0, 0, 0))
        : model(model), _pos(pos), _euler(euler)  {}
    void draw(const Window& win) {
        if (!_visible) return;
        _update_mat();
        glm::mat4 MVP        = win._vp* _posmatrix;
        glUseProgram(win._program_id);
        glUniformMatrix4fv(win._matrix_location, 1, GL_FALSE, &MVP[0][0]);
        model->draw();
    }
    void position(const glm::vec3& pos, const glm::vec3& euler = glm::vec3(0, 0, 0)) {
        _pos = pos;
        _euler = euler;
    }
    glm::vec3 get_pos() {
        return _pos;
    }
    bool visible() {return _visible;}
    void visible(bool flag) {_visible = flag;}
    virtual void on_collision(int id) {}
protected:
    virtModel* model;
    void _update_mat(){
        _posmatrix = glm::translate(glm::mat4(1.0f), _pos) * glm::orientate4(_euler);
        //_posmatrix = glm::translate(glm::orientate4(_euler), _pos);
    }
    //std::set<int> rivals;
    bool _visible = true;
    glm::vec3 _pos;
    glm::vec3 _euler;
    glm::mat4 _posmatrix = glm::mat4(1.0f);
};

class SphereTriangleModel : public virtModel{
public:
    SphereTriangleModel(glm::vec3 a, glm::vec3 b, glm::vec3 c): a(a), b(b), c(c) {
        GLfloat g_vertex_buffer_data[] = {
            a[0], a[1], a[2],
            b[0], b[1], b[2],
            c[0], c[1], c[2]
        };
        GLfloat g_color_buffer_data[] = {
            1.0f, 0.1f, 0.1f,
            1.0f, 0.6f, 0.0f,
            1.0f, 0.0f, 0.5f
        };
        glGenBuffers(1, &_vbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _vbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

        glGenBuffers(1, &_cbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, _cbuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
    }

    void draw() {
        if (childA) {
            childA->draw();
            childB->draw();
            childC->draw();
            childX->draw();
        }
        else {
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, _vbuffer);
            glVertexAttribPointer(
                        0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                        3,                  // size
                        GL_FLOAT,           // type
                        GL_FALSE,           // normalized?
                        0,                  // stride
                        (void*)0            // array buffer offset
                        );

            // 2nd attribute buffer : colors
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, _cbuffer);
            glVertexAttribPointer(
                        1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
                        3,                                // size
                        GL_FLOAT,                         // type
                        GL_FALSE,                         // normalized?
                        0,                                // stride
                        (void*)0                          // array buffer offset
                        );

            // Draw the triangle !
            glDrawArrays(GL_TRIANGLES, 0, 3); // 12*3 indices starting at 0 -> 12 triangles

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);
        }
    }

    void split() {
        if (childA) {
            childA->split();
            childB->split();
            childC->split();
            childX->split();
        }
        else {
            childA = new SphereTriangleModel(a, glm::normalize(0.5f*(a+b)), glm::normalize(0.5f*(a+c)));
            childB = new SphereTriangleModel(glm::normalize(0.5f*(a+b)), b, glm::normalize(0.5f*(b+c)));
            childC = new SphereTriangleModel(glm::normalize(0.5f*(a+c)), glm::normalize(0.5f*(b+c)), c);
            childX = new SphereTriangleModel(glm::normalize(0.5f*(b+c)), glm::normalize(0.5f*(a+c)), glm::normalize(0.5f*(a+b)));
        }
    }

    void unsplit() {
        if(childA) {
            if(childA->childA) {
                childA->unsplit();
                childB->unsplit();
                childC->unsplit();
                childX->unsplit();
            }
            else {
                delete childA;
                childA = nullptr;
                delete childB;
                childB = nullptr;
                delete childC;
                childC = nullptr;
                delete childX;
                childX = nullptr;
            }
        }
    }

    ~SphereTriangleModel() {
        glDeleteBuffers(1, &_vbuffer);
        glDeleteBuffers(1, &_cbuffer);
        delete childA, childB, childC, childX;
    }
private:
    GLuint _vbuffer, _cbuffer;
    glm::vec3 a, b, c;
    SphereTriangleModel* childA = nullptr;
    SphereTriangleModel* childB = nullptr;
    SphereTriangleModel* childC = nullptr;
    SphereTriangleModel* childX = nullptr;
};

class Sphere4Model : public virtModel {
public:
    Sphere4Model() {
        glm::vec3 a(0, 1, 0);
        glm::vec3 b(-sqrt(2)/3.0, -1.0/3, -sqrt(2.0/3));
        glm::vec3 c(-sqrt(2)/3.0, -1.0/3, sqrt(2.0/3));
        glm::vec3 d(2*sqrt(2)/3, -1.0/3, 0);
        childA = new SphereTriangleModel(b, c, d);
        childB = new SphereTriangleModel(a, c, d);
        childC = new SphereTriangleModel(a, b, d);
        childD = new SphereTriangleModel(a, b, c);
    }
    void draw() {
        childA->draw();
        childB->draw();
        childC->draw();
        childD->draw();
    }
    void split() {
        childA->split();
        childC->split();
        childB->split();
        childD->split();
    }
    void unsplit() {
        childA->unsplit();
        childB->unsplit();
        childC->unsplit();
        childD->unsplit();}
private:
    SphereTriangleModel* childA = nullptr;
    SphereTriangleModel* childB = nullptr;
    SphereTriangleModel* childC = nullptr;
    SphereTriangleModel* childD = nullptr;
};

class CollisionController {
public:
    void create_group(int id, std::initializer_list<int> rivals) {
        _groups[id] = std::map<Unit*, glm::vec3>();
        _group_rivals[id] = std::set<int>(rivals);
    }
    void add_to_group(int id, Unit* unit, glm::vec3 box) {
        _groups[id][unit] = box;
    }
    void handle_collisions() {
        for (auto const& map_ : _groups) {
            int current_group_id = map_.first;
            const std::map<Unit*, glm::vec3>& units = map_.second;
            const std::set<int>& rivals = _group_rivals[current_group_id];
            for (auto const& current_unit : units) {
                for (int rival_group_id : rivals) {
                    const std::map<Unit*, glm::vec3>& rival_units = _groups[rival_group_id];
                    for (auto const& rival_unit: rival_units) {
                        bool collision = _check_collision(
                                    _unit_to_rect(current_unit),
                                    _unit_to_rect(rival_unit));
                        if (collision) {
                            std::cout << "collision found!!!" << std::endl;
                            current_unit.first->on_collision(rival_group_id);
                        }
                    }
                }
            }
        }
    }
private:
    struct rect {
        GLfloat x_, _x;
        GLfloat y_, _y;
        GLfloat z_, _z;
    };
    rect _unit_to_rect(std::pair<Unit*, glm::vec3> unit) {
        rect rect;
        glm::vec3 pos = unit.first->get_pos();
        rect._x = pos.x + unit.second.x/2;
        rect.x_ = pos.x - unit.second.x/2;
        rect._y = pos.y + unit.second.y/2;
        rect.y_ = pos.y - unit.second.y/2;
        rect._z = pos.z + unit.second.z/2;
        rect.z_ = pos.z - unit.second.z/2;
        return rect;
    }
    bool static _check_collision(rect f, rect s) {
        return  (f._x > s.x_) && (f.x_ < s._x) &&
                (f._y > s.y_) && (f.y_ < s._y) &&
                (f._z > s.z_) && (f.z_ < s._z);
    }
    std::map<int, std::map<Unit*, glm::vec3> > _groups ;
    std::map<int, std::set<int> > _group_rivals;
};

class Loop {
public:
    Loop(Window* window): _win(window) {}
    void add_func(std::function<void(Loop*)> func) {
        _func_list.push_back(func);
    }
    void run(float target_fps) {
        auto last_frame_start = std::chrono::steady_clock::now();
        do {
            auto frame_start = std::chrono::steady_clock::now();
            _delta = std::chrono::duration_cast<std::chrono::microseconds> (frame_start - last_frame_start).count();
            last_frame_start = frame_start;
            if (_show_fps) std::cout << "fps: " << 1000000.0 / _delta << std::endl;

            _win->start_frame();
            for (auto func : _func_list) func(this);
            if (_check_collisions) collision_controller.handle_collisions();
            _win->end_frame();

            auto frame_end = std::chrono::steady_clock::now();
            auto frame_duration = std::chrono::duration_cast<std::chrono::microseconds> (frame_end - frame_start).count();
            auto sleep_required = 1000000.0 / target_fps  - frame_duration;
            //WARNING not cross-platform solution;
            if (sleep_required > 0) usleep(sleep_required);
        }
        while(_win->exit_check());
    }
    void show_fps(bool flag) {_show_fps = flag;}
    void handle_collisions(bool flag) {_check_collisions = flag;}
    void time_scale(float scale) {_time_scale = scale;}
    Window* window() {return _win;}
    long long scaled_delta() {return _delta*_time_scale;}
    long long real_delata() {return _delta;}
    CollisionController collision_controller;
private:
    long long _delta;
    std::vector<std::function<void(Loop*)>> _func_list;
    Window* _win;
    bool _show_fps = false;
    bool _check_collisions = false;
    float _time_scale = 1.0;
};


#endif // ENGINE_H
