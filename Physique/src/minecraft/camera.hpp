#ifndef CAMERA_HPP_
#define CAMERA_HPP_

#include "mat.h"
#include "draw.h"
#include "image_io.h"

class b_Camera {

public:
    b_Camera();
    b_Camera(const Point &o);
    b_Camera(const Point &o, const Image &f, const int &c);
    ~b_Camera();

    Transform view() { return v; }
    Transform projection() { return p; }

    void moveForward();
    void moveBackward();
    void turnRight();
    void turnLeft();
    Point position();

private:
    Point center;
    Vector direction;
    Transform v;
    Transform p;
    Image field;
    int coeff_height;
};

#endif // CAMERA_HPP_
