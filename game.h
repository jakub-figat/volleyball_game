#pragma once

#include "constants.cpp"
#include <random>
#include <print>
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
    // random ball drop
    void serve() {
        shape.setPosition(BALL_START_POSITION);

        std::uniform_real_distribution<float> distrib_x(-200, 200);
        std::uniform_real_distribution<float> distrib_y(-1, 0);
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


void resolve_collisions(std::array<Rectangle, 2>& rectangles, sf::RectangleShape& net, Ball& ball, const std::array<sf::RectangleShape, 5>& walls) {
    resolve_rectangles_with_solids(rectangles, walls);
    resolve_rectangles_with_floor(rectangles);
    resolve_ball_with_solids(ball, walls);
    resolve_ball_with_floor(ball);
    resolve_ball_with_rectangles(ball, rectangles);
}


void resolve_rectangles_with_solids(std::array<Rectangle, 2>& rectangles, const std::array<sf::RectangleShape, 5>& walls) {
    for (auto& rectangle: rectangles) {
        for (const auto& wall : walls) {
            if (auto overlap = get_overlap(rectangle.shape, wall)) {
                
            }
        }
    }
}

void resolve_rectangles_with_floor(std::array<Rectangle, 2>& rectangles);
void resolve_ball_with_solids(Ball& ball, const std::array<sf::RectangleShape, 5>& walls);
void resolve_ball_with_floor(Ball& ball);
void resolve_ball_with_rectangles(Ball& ball, std::array<Rectangle, 2>& rectangles);


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

void do_ball_collisions(Ball& ball, sf::RectangleShape& net, std::array<Rectangle, 2>& rectangles, unsigned int& score_1, unsigned int& score_2) {
    // when border or net is hit, reverse the vector
    auto [ball_x, ball_y] = ball.shape.getPosition();
    auto [ball_max_x, ball_max_y] = ball.get_max_position();

    // first, collision with floor and map borders
    if (ball_x < 0.0f) {
        ball.velocity_x = -ball.velocity_x;
        ball.shape.move({-ball_x, 0.0});
    } else if (ball_max_x > WIDTH) {
        ball.velocity_x = -ball.velocity_x;
        ball.shape.move({WIDTH - ball_max_x, 0.0});
    }

    if (ball_max_y > FLOOR_HEIGHT) {
        (ball_x > MIDDLE ? score_1 : score_2) += 1;
        ball.serve();
    }

    if (ball_y < 0.0f) {
        ball.velocity_y = -ball.velocity_y;
        ball.shape.move({0.0, -ball_y});
    }

    // net collision has to be different
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

    } else if (ball_y + ball_radius >= net_y) {
        if (ball_max_x > net_x && ball_x < net_x) {
            ball.velocity_x = -ball.velocity_x;
            ball.shape.move({net_x - ball_max_x, 0.0});
            
        } else if (ball_x < net_max_x && ball_max_x > net_max_x) {
            ball.velocity_x = -ball.velocity_x;
            ball.shape.move({net_max_x - ball_x, 0.0});
        }
    }

    // collision with top of paddle, influence the ball velocity always in the same way the paddle is moving
    for (auto& rectangle : rectangles) {
        auto [rectangle_x, rectangle_y] = rectangle.shape.getPosition();
        auto rectangle_max_x = rectangle.get_max_position().x;

        // paddle top hit, no need to check side hit when below head
        if (ball_max_y > rectangle_y && ball_y + ball_radius < rectangle_y && (ball_center > rectangle_x - ball_radius && ball_center < rectangle_max_x + ball_radius)) {
            // for now, if paddle is moving opposite way or stationary, just bounce the ball, if velocities are in the same direction, speedup ball slightly
            if (ball.velocity_x * rectangle.velocity_x > 0) {
                // TODO: need to come up with formula that is satisfying for gameplay
                ball.velocity_x = ball.velocity_x * HIT_FORCE;
            }

            ball.velocity_y = -ball.velocity_y;
            ball.velocity_y += rectangle.velocity_y * 0.3f;
            ball.shape.move({0.0, rectangle_y - ball_max_y});
        }
    }
}
