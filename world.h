#pragma once

#include "camera.h"
#include <math.h>
#include <limits>


constexpr double infinity = std::numeric_limits<double>::infinity();


class Sphere {
public:
    Point center;
    double radius;
    Color color;
};


class HitRecord {
public:
    Point point;
    Vec3 surface_normal;
    double t;
    Color color;
};


class World {
private:
    std::vector<Sphere> spheres;

public:
    Vec3 light_direction;
    Color light_color;
    
    World(std::vector<Sphere> s, Vec3 l, Color c): spheres(std::move(s)), light_direction(std::move(l)), light_color(std::move(c)) {}

    std::optional<HitRecord> intersect(const Ray& ray) const {
        double t {infinity};
        Color color;
        Point shape_center;

        for (const auto& sphere : spheres) {
            Vec3 v = static_cast<Vec3>(ray.origin) - sphere.center;

            // since a=1 because ray direction is normalized
            double b = 2*v.dot(ray.direction);
            double c = v.dot(v) - sphere.radius * sphere.radius;
            double discriminant = b * b - 4 * c;

            if (discriminant < 0) {
                continue;
            }

            double t1 = (-b - std::sqrt(discriminant)) / 2;
            double t2 = (-b + std::sqrt(discriminant)) / 2;
            double root;

            if (t1 < 0 && t2 < 0) {
                continue;
            } else if (t1 < 0) {
                root = t2;
            } else if (t2 < 0) {
                root = t1;
            } else {
                root = std::min(t1, t2);
            }

            t = std::min(t, root);
            shape_center = sphere.center;
            color = sphere.color;
        }

        if (t == infinity) {
            return std::nullopt;
        }


        auto hit_point = ray.at(t);
        return HitRecord {hit_point, (hit_point - shape_center).normalized(), t, color};
    }
};