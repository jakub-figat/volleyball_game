#pragma once

#include "constants.cpp"
#include <random>
#include <print>
#include <vector>
#include <SFML/Graphics.hpp>


struct Rectangle {
    sf::RectangleShape shape;
    float velocity_x, velocity_y;

    sf::Vector2f get_max_position() const {
        return shape.getPosition() + shape.getSize();
    }
};


struct Overlap {
    float x, y;
};


static std::mt19937 gen(std::random_device{}());


struct Ball : Rectangle {
    void serve() {
        shape.setPosition(BALL_START_POSITION);

        std::uniform_real_distribution<float> distrib_x(-200, 200);
        std::uniform_real_distribution<float> distrib_y(-10, 10);
        velocity_x = distrib_x(gen);
        velocity_y = distrib_y(gen);
    }
};


void compute_velocities(float dt, std::array<Rectangle, 2>& rectangles, Ball& ball) {
    for (auto& rectangle : rectangles) {
        rectangle.velocity_x = 0;
        rectangle.velocity_y += GRAVITY * dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            rectangle.velocity_x = -PADDLE_SPEED;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            rectangle.velocity_x = PADDLE_SPEED;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && rectangle.get_max_position().y == FLOOR_HEIGHT) {
            rectangle.velocity_y = JUMP_VELOCTIY;
        }
    }

    ball.velocity_y += GRAVITY * dt;
}

void make_move(float dt, std::array<Rectangle, 2>& rectangles, Ball& ball) {
    for (auto& rectangle : rectangles) {
        rectangle.shape.move({rectangle.velocity_x * dt, rectangle.velocity_y * dt});
    }
    ball.shape.move({ball.velocity_x * dt, ball.velocity_y * dt});
}


std::optional<Overlap> get_overlap(const sf::RectangleShape& rectangle, const sf::RectangleShape& other) {
    auto [rectangle_x, rectangle_y] = rectangle.getPosition();
    auto [rectangle_max_x, rectangle_max_y] = rectangle.getPosition() + rectangle.getSize();

    auto [other_x, other_y] = other.getPosition();
    auto [other_max_x, other_max_y] = other.getPosition() + other.getSize();


    float overlap_x = std::min(rectangle_max_x, other_max_x) - std::max(rectangle_x, other_x);
    float overlap_y = std::min(rectangle_max_y, other_max_y) - std::max(rectangle_y, other_y);

    if (overlap_x <= 0 || overlap_y <= 0) {
        return std::nullopt;
    }

    return Overlap {overlap_x, overlap_y};
}


void resolve_rectangles_with_solids(std::array<Rectangle, 2>& rectangles, const std::vector<sf::RectangleShape>& walls) {
    for (auto& rectangle: rectangles) {
        for (const auto& wall : walls) {
            if (auto overlap = get_overlap(rectangle.shape, wall)) {
                if (overlap->x > overlap->y) {
                    auto sign = rectangle.shape.getPosition().y < wall.getPosition().y ? -1.0f : 1.0f;
                    
                    rectangle.shape.move({0.0f, sign * overlap->y});
                    rectangle.velocity_y = 0;
                } else {
                    auto sign = rectangle.shape.getPosition().x < wall.getPosition().x ? -1.0f : 1.0f;

                    rectangle.shape.move({sign * overlap->x, 0.0f});
                    rectangle.velocity_x = 0;
                }
            }
        }
    }
}


void resolve_ball_with_solids(Ball& ball, const std::vector<sf::RectangleShape>& walls) {
    for (const auto& wall : walls) {
        if (auto overlap = get_overlap(ball.shape, wall)) {
            if (overlap->x > overlap->y) {
                auto sign = ball.shape.getPosition().y < wall.getPosition().y ? -1.0f : 1.0f;
                
                ball.shape.move({0.0f, sign * overlap->y});
                ball.velocity_y = -ball.velocity_y;
            } else {
                auto sign = ball.shape.getPosition().x < wall.getPosition().x ? -1.0f : 1.0f;

                ball.shape.move({sign * overlap->x, 0.0f});
                ball.velocity_x = -ball.velocity_x;
            }
        }
    }
}


void resolve_ball_with_rectangles(Ball& ball, const std::array<Rectangle, 2>& rectangles) {
    for (const auto& rectangle : rectangles) {
        if (auto overlap = get_overlap(ball.shape, rectangle.shape)) {
            if (overlap->x > overlap->y && ball.shape.getPosition().y < rectangle.shape.getPosition().y) {
                ball.shape.move({0.0, -overlap->y});
                ball.velocity_y = -ball.velocity_y * 0.9f + rectangle.velocity_y * 0.6f;
                ball.velocity_x = std::clamp(ball.velocity_x + rectangle.velocity_x, -PADDLE_SPEED, PADDLE_SPEED);
            }
        }
    }
}


void resolve_ball_with_floor(Ball& ball, const sf::RectangleShape& floor, unsigned int& score_1, unsigned int& score_2) {
    if (auto overlap = get_overlap(ball.shape, floor)) {
        (ball.shape.getPosition().x > MIDDLE ? score_1 : score_2) += 1;
        ball.serve();
    }
}


void resolve_collisions(
    std::array<Rectangle, 2>& rectangles,
    Ball& ball,
    const std::vector<sf::RectangleShape>& walls,
    const sf::RectangleShape& floor,
    unsigned int& score_1,
    unsigned int& score_2
) {
    resolve_rectangles_with_solids(rectangles, walls);
    resolve_rectangles_with_solids(rectangles, {floor});

    resolve_ball_with_solids(ball, walls);
    resolve_ball_with_rectangles(ball, rectangles);
    
    resolve_ball_with_floor(ball, floor, score_1, score_2);
}
