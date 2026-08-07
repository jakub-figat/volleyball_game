#include "ppm.h"
#include "renderer.h"
#include "camera.h"
#include <iostream>
#include <print>



int main() {
    int image_height = 720;
    double aspect_ratio = 16.0 / 9.0;
    int image_width = static_cast<int>(image_height * aspect_ratio);
    double fov = 90;

    Camera camera {
            Point {0, 0, -3},
            Point {0, 0, 0},
            aspect_ratio,
            fov
    };
    Sphere s1 {Point{-100, -20, 100}, 20, Color {255, 120, 0}};
    Sphere s2 {Point{100, -20, 100}, 20, Color {70, 150, 200}};
    Sphere s3 {Point{0, 25, 100}, 20, Color {150, 150, 200}};

    Vec3 sun_direction {1, -1, 1};

    World world {std::vector {s1, s2, s3}, sun_direction.normalized(), Color {200, 200, 200}};

    Renderer renderer {std::move(camera), std::move(world)};
    auto result = renderer.generate_pixels_from_camera(image_width, image_height);

    write_ppm(result, image_width, image_height);
    return 0;
}