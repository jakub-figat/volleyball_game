#pragma once

#include "camera.h"


class Sphere {
public:
    Point center;
    double radius;
};


class HitRecord {
public:
    Point point;
    Vec3 surface_normal;
    double t;
};


class World {
private:
    Sphere sphere;

public:
    World(Sphere s): sphere(s) {}

    std::optional<HitRecord> intersect(const Ray& ray) {
        Vec3 v = static_cast<Vec3>(ray.origin) - sphere.center;

        // since a=1 because ray direction is normalized
        double b = 2*v.dot(ray.direction);
        double c = v.dot(v) - sphere.radius * sphere.radius;
        double discriminant = b * b - 4 * c;

        if (discriminant < 0) {
            return std::nullopt;
        }

        double t;
        if (discriminant == 0) {
            t = -b / 2;
        } else {
            double t1 = (-b - std::sqrt(discriminant)) / 2;
            double t2 = (-b + std::sqrt(discriminant)) / 2;

            if (t1 < 0 && t2 < 0) {
                return std::nullopt;
            } else if (t1 < 0) {
                t = t2;
            } else if (t2 < 0) {
                t = t1;
            } else {
                t = std::min(t1, t2);
            }
        }
        auto hit_point = ray.at(t);
        return HitRecord {hit_point, (hit_point - sphere.center).normalized(), t};
    }
};