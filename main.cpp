#include "game.h"
#include <array>
#include <print>
#include <SFML/Graphics.hpp>


// do I separate velocity and distance computation for collision checks?
// should this function always increase velocity_y and collision function should always set it to 0 later?
// or different compute_position function?

// is compute_velocity -> make_move -> do collisions good order? Where last step sticks  border of one rectangle to the other and sets velocity to 0?


// so far, keys, jumps, collisions with net and borders look good
// there definitely is a collision bug with net if fps is set to very low
// this could be fixed by calculating move trajectory and if checking if movement will not occur through the net
// bug happens because there is much more movement per frame with low fps and at no point in time Rectangle borders cross the net borders



int main() {
    sf::RenderWindow window(sf::VideoMode({static_cast<int>(WIDTH), static_cast<int>(HEIGHT)}), "vgame", sf::Style::Close);
    sf::Vector2f rectangle_shape {10.f, 50.f};

    std::array<Rectangle, 2> rectangles {
        Rectangle {sf::RectangleShape {rectangle_shape}, 200.0, 0.0},
        Rectangle {sf::RectangleShape {rectangle_shape}, 200.0, 0.0},
    };

    rectangles[0].shape.setPosition({MIDDLE - 200, FLOOR_HEIGHT - rectangle_shape.y});
    rectangles[1].shape.setPosition({MIDDLE + 200, FLOOR_HEIGHT - rectangle_shape.y});

    rectangles[0].shape.setFillColor(sf::Color::White);
    rectangles[1].shape.setFillColor(sf::Color::White);

    Ball ball {sf::RectangleShape{{BALL_SIZE, BALL_SIZE}}, 0.0, 0.0};
    ball.shape.setFillColor(sf::Color::White);
    ball.shape.setPosition({MIDDLE - BALL_SIZE / 2.0f, 200.0});
    

    sf::RectangleShape line(sf::Vector2f(WIDTH, 1.0f));
    line.setPosition({0, FLOOR_HEIGHT});
    line.setFillColor(sf::Color::White);

    sf::RectangleShape net({10.0f, 100.0f});
    net.setPosition({MIDDLE - 5.0f, FLOOR_HEIGHT - net.getSize().y});
    net.setFillColor(sf::Color::White);


    window.setFramerateLimit(120);
    sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        auto dt = clock.restart().asSeconds();
        compute_velocities(dt, rectangles, ball);
        make_move(dt, rectangles, ball);
        do_rectangle_collisions(rectangles, net);
        do_ball_collisions(ball, net);

        window.clear(sf::Color::Black);

        for (const auto& rectangle : rectangles) {
            window.draw(rectangle.shape);
        }

        window.draw(line);
        window.draw(net);
        window.draw(ball.shape);
        window.display();
    }

    return 0;
}