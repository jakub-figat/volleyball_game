#pragma once

#include "color.h"
#include "camera.h"
#include "world.h"
#include <vector>


class Renderer {
private:
    Camera camera;
    World world;

public:
    Renderer(Camera c, World w): camera(std::move(c)), world(std::move(w)) {}

    std::vector<Color> generate_pixels_from_camera(
        int width,
        int height
    ) {
        std::vector<Color> pixel_result;
        pixel_result.resize(static_cast<size_t>(width*height));

        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                double norm_x = (i + 0.5) / width;
                double norm_y = (j + 0.5) / height;
                double u = 2 * norm_x - 1;
                double v = 1 - 2 * norm_y;
                
                auto ray = camera.cast_ray(u, v);
                auto hit_record = world.intersect(ray);
                auto color = hit_record.has_value() ? Color{0, 0, 0} : Color{135, 206, 235};

                pixel_result[static_cast<size_t>(j * width + i)] = color;
            }
        }

        return pixel_result;
    }
};