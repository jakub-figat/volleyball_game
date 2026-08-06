#pragma once

#include "color.h"
#include <vector>
#include <iostream>
#include <fstream>


int write_ppm(const std::vector<Color>& pixels, int width, int height) {
    std::ofstream file{"image.ppm"};

    if (!file.is_open()) {
        std::cerr << "Failed to open file";
        return -1;
    }

    file << "P3\n" << width << " " << height << "\n255\n";
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            const auto& pixel = pixels[static_cast<size_t>(j * width + i)];
            file << pixel.r << " " << pixel.g << " " << pixel.b << "\n";
        }
    }

    return 0;
}