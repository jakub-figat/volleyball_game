#pragma once

#include "camera.h"
#include "world.h"
#include <math.h>
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
        Color background{135, 206, 235};

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
                Color color;

                if (hit_record.has_value()) {
                    color = hit_record->color;

                    auto light_intensity = std::max(0.0, hit_record->surface_normal.dot(-world.light_direction));

                    Ray shadow_ray {hit_record->point + (-world.light_direction)*0.05, -world.light_direction};
                    auto shadow_factor = world.intersect(shadow_ray).has_value() ? 0.0 : 1.0;
                    auto normalized_color = (color / 255.0) * (world.light_color / 255.0);

                    color = normalized_color * 255.0 * shadow_factor * light_intensity;
                } else {
                    color = background;
                }

                pixel_result[static_cast<size_t>(j * width + i)] = color;
            }
        }

        return pixel_result;
    }
};