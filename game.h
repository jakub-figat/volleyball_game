#pragma once

#include "constants.cpp"
#include <SFML/Graphics.hpp>

struct Rectangle {
    sf::RectangleShape shape;
    float velocity_x, velocity_y;

    sf::Vector2f get_max_position() const {
        return shape.getPosition() + shape.getSize();
    }
};

struct Ball : Rectangle {
    
};


void compute_velocities(float dt, std::array<Rectangle, 2>& rectangles, Ball& ball) {
    for (auto& rectangle : rectangles) {
        rectangle.velocity_y += GRAVITY * dt;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && rectangle.get_max_position().y == FLOOR_HEIGHT) {
            rectangle.velocity_y = JUMP_VELOCTIY;
        }
    }

    ball.velocity_y += GRAVITY * dt;
}

void make_move(float dt, std::array<Rectangle, 2>& rectangles, Ball& ball) {
    for (auto& rectangle : rectangles) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            rectangle.shape.move({-rectangle.velocity_x * dt, 0.0});

            ball.shape.move({-rectangle.velocity_x * dt, 0.0});
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            rectangle.shape.move({rectangle.velocity_x * dt, 0.0});
            ball.shape.move({rectangle.velocity_x * dt, 0.0});
        }
        rectangle.shape.move({0, rectangle.velocity_y * dt});
    }

    ball.shape.move({0.0, ball.velocity_y * dt});
}


// so maybe detecting collision in y axis could really set vel_y to 0
void do_rectangle_collisions(std::array<Rectangle, 2>& rectangles, sf::RectangleShape& net) {
    // collision in x: revert x position so borders of both rectangles stick to each other, dont pass through
    // collision in y: do the same, but also set vel_y to 0
    // this seems to work fairly fine?

    for (auto& rectangle : rectangles) {
        auto [rectangle_x, rectangle_y] = rectangle.shape.getPosition();
        auto [rectangle_max_x, rectangle_max_y] = rectangle.get_max_position();

        // first, collision with floor and map borders
        if (rectangle_x < 0.0f) {
            rectangle.shape.move({-rectangle_x, 0.0});
        } else if (rectangle_max_x > WIDTH) {
            rectangle.shape.move({WIDTH - rectangle_max_x, 0.0});
        }

        if (rectangle_max_y > FLOOR_HEIGHT) {
            rectangle.velocity_y = 0.0;
            rectangle.shape.move({0.0, FLOOR_HEIGHT - rectangle_max_y});
        } else if (rectangle_y < 0.0f) {
            rectangle.velocity_y = 0.0;
            rectangle.shape.move({0.0, -rectangle_y});
        }

        // net collision
        // maybe there should be generic function to iterate each rectangle*rectangle pair? but not for now
        auto net_x = net.getPosition().x;
        auto net_max_x = (net.getPosition() + net.getSize()).x;

        if (rectangle_max_x > net_x && rectangle_x < net_x) {
            rectangle.shape.move({net_x - rectangle_max_x, 0.0});
        } else if (rectangle_x < net_max_x && rectangle_max_x > net_max_x) {
            rectangle.shape.move({net_max_x - rectangle_x, 0.0});
        }
    }
}

void do_ball_collisions(Ball& ball, sf::RectangleShape& net) {
    // when border or net is hit, reverse the vector
    auto [ball_x, ball_y] = ball.shape.getPosition();
    auto [ball_max_x, ball_max_y] = ball.get_max_position();

    // first, collision with floor and map borders
    if (ball_x < 0.0f) {
        ball.velocity_x = -ball.velocity_x;
        ball.shape.move({-ball_x, 0.0});
    } else if (ball_x > WIDTH) {
        ball.velocity_x = -ball.velocity_x;
        ball.shape.move({WIDTH - ball_max_x, 0.0});
    }

    // hit against the ground, lose some velocity
    if (ball_max_y > FLOOR_HEIGHT) {
        ball.velocity_y = -ball.velocity_y * 0.2f;
        ball.shape.move({0.0, FLOOR_HEIGHT - ball_max_y});
    }

    if (ball_y < 0.0f) {
        ball.velocity_y = -ball.velocity_y;
        ball.shape.move({0.0, -ball_y});
    }

    // // net collision has to be different
    // when below net_y, check from sides
    // when above, check the ball bottom

    auto [net_x, net_y] = net.getPosition();
    auto [net_max_x, net_max_y] = net.getPosition() + net.getSize();
    
    auto ball_radius = BALL_SIZE / 2.0f;
    auto ball_center = ball_x + ball_radius;

    // net top hit
    // maybe its easiest to check if ball center is inside range of [net_x - ball_width / 2, max_net_x + ball_width / 2]
    if (ball_max_y > net_y && ball_y + ball_radius < net_y && (ball_center > net_x - ball_radius && ball_center < net_max_x + ball_radius)) {
        ball.velocity_y = -ball.velocity_y;
        ball.shape.move({0.0, net_y - ball_max_y});
    }

    // if (ball_max_x > net_x && ball_x < net_x) {
    //     ball.shape.move({net_x - ball_max_x, 0.0});
    // } else if (ball_x < net_max_x && ball_max_x > net_max_x) {
    //     ball.shape.move({net_max_x - ball_x, 0.0});
    // }
}