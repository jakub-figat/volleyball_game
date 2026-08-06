#pragma once

#include "vec3.h"


constexpr double PI = 3.14159265358979323846;


class Ray {
public:
    Point origin;
    Vec3 direction;

    Ray(Point o, Vec3 d): origin(o), direction(d) {}

    Point at(double t) const {
        return origin + t * direction;
    }
};

class Camera {
private:
    Point position;
    Point look_at;

    Vec3 forward;
    Vec3 true_up;
    Vec3 right;

    double aspect_ratio;
    double fov_scale;

public:
    Camera(Point p, Point l, double ar, double fov): position(p), look_at(l), aspect_ratio(ar) {
        double radians = fov * PI / 180.0;
        fov_scale = std::tan(radians / 2.0);

        Vec3 up {0, 1, 0};
        forward = (look_at - position).normalized();
        right = up.cross(forward).normalized();
        true_up = forward.cross(right).normalized();
    }

    // u, v passed here must be normalized by width and height first [-1, 1]
    Ray cast_ray(double u, double v) {
        double pos_x = u * aspect_ratio * fov_scale; 
        double pos_y = v * fov_scale;

        return Ray {position, (pos_x * right + pos_y * true_up + forward).normalized()};
    }
};