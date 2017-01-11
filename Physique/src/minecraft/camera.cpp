#include "camera.hpp"


b_Camera::b_Camera() {
    center = Point(0.f, 0.f, 0.f);
    direction = Vector(1.f, 0.f, 0.f);
    Point to(direction.x+center.x, direction.y+center.y, direction.z+center.z);
    v = Lookat(center, to, Vector(0.f, 1.f, 0.f));
    p = Perspective(45, 1.f, 0.f, 1.f);
}

b_Camera::b_Camera(const Point &o) {
    center = o;
    direction = Vector(1.f, 0.f, 0.f);
    Point to(direction.x+center.x, direction.y+center.y, direction.z+center.z);
    v = Lookat(center, to, Vector(0.f, 1.f, 0.f));
    p = Perspective(45, 1.f, 5.f, 250.f);
}

b_Camera::b_Camera(const Point &o, const Image &f, const int &c) {
    center = o;
    direction = Vector(1.f, 0.f, 0.f);
    Point to(direction.x+center.x, direction.y+center.y, direction.z+center.z);
    v = Lookat(center, to, Vector(0.f, 1.f, 0.f));
    p = Perspective(45, 1.f, 2.f, 250.f);
    field = f;
    coeff_height = c;
}

b_Camera::~b_Camera() {}


void b_Camera::moveForward() {
    center = Point(center.x+(direction.x/5), 0, center.z+(direction.z/5));
    center.y = 5+coeff_height*field(center.x, center.z).r;
    Point to(direction.x+center.x, direction.y+center.y, direction.z+center.z);
    v = Lookat(center, to, Vector(0.f, 1.f, 0.f));
}

void b_Camera::moveBackward() {
    center = Point(center.x-(direction.x/5) , 0, center.z-(direction.z/5) );
    center.y = 5+coeff_height*field(center.x, center.z).r;
    Point to(direction.x+center.x, direction.y+center.y, direction.z+center.z);
    v = Lookat(center, to, Vector(0.f, 1.f, 0.f));
}

void b_Camera::turnRight() {
    Transform r = RotationY(-1);
    direction = r(direction);
    Point to(direction.x+center.x, direction.y+center.y, direction.z+center.z);
    v = Lookat(center, to, Vector(0.f, 1.f, 0.f));
}

void b_Camera::turnLeft() {
    Transform r = RotationY(1);
    direction = r(direction);
    Point to(direction.x+center.x, direction.y+center.y, direction.z+center.z);
    v = Lookat(center, to, Vector(0.f, 1.f, 0.f));

}

Point b_Camera::position() { return center; }
