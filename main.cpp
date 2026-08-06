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
            static_cast<double>(image_width),
            static_cast<double>(image_height),
            fov
    };
    Sphere s {Point{0, 0, 50}, 10};
    World world {s};

    Renderer renderer {camera, world};
    auto result = renderer.generate_pixels_from_camera(image_width, image_height);

    write_ppm(result, image_width, image_height);
    return 0;
}