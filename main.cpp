#include "game.h"
#include <array>
#include <print>
#include <thread>
#include <chrono>
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
    sf::Font font("Pixel Game.otf");

    sf::Text main_text(font);
    main_text.setString("Press ENTER to start");

    sf::Text text_1(font);
    sf::Text text_2(font);

    text_1.setCharacterSize(45);
    text_2.setCharacterSize(45);
    main_text.setCharacterSize(45);

    text_1.setFillColor(sf::Color::White);
    text_2.setFillColor(sf::Color::White);
    main_text.setFillColor(sf::Color::White);

    text_1.move({30.0, 30.0});
    text_2.move({30.0, 60.0});
    main_text.move({30.0f, 90.0f});

    sf::Vector2f rectangle_shape {20.f, 60.f};

    std::array<Rectangle, 2> rectangles {
        Rectangle {sf::RectangleShape {rectangle_shape}, PADDLE_SPEED, 0.0},
        Rectangle {sf::RectangleShape {rectangle_shape}, PADDLE_SPEED, 0.0},
    };

    rectangles[0].shape.setPosition({MIDDLE - 200, FLOOR_HEIGHT - rectangle_shape.y});
    rectangles[1].shape.setPosition({MIDDLE + 200, FLOOR_HEIGHT - rectangle_shape.y});

    rectangles[0].shape.setFillColor(sf::Color::White);
    rectangles[1].shape.setFillColor(sf::Color::White);

    Ball ball {sf::RectangleShape{{BALL_SIZE, BALL_SIZE}}, 0.0, 0.0};
    ball.shape.setFillColor(sf::Color::White);
    ball.shape.setPosition(BALL_START_POSITION);
    ball.serve();
    

    sf::RectangleShape line(sf::Vector2f(WIDTH, 1.0f));
    line.setPosition({0, FLOOR_HEIGHT});
    line.setFillColor(sf::Color::White);

    sf::RectangleShape net({10.0f, 100.0f});
    net.setPosition({MIDDLE - 5.0f, FLOOR_HEIGHT - net.getSize().y});
    net.setFillColor(sf::Color::White);


    std::array<sf::RectangleShape, 5> walls {
        sf::RectangleShape{{1.0f, HEIGHT}},
        sf::RectangleShape{{1.0f, HEIGHT}},
        sf::RectangleShape{{WIDTH, 1.0f}},
        sf::RectangleShape{{WIDTH, 1.0f}},
        net
    };

    walls[1].setPosition({WIDTH, 0.0});
    walls[3].setPosition({0.0, HEIGHT});


    window.setFramerateLimit(120);
    sf::Clock clock;
    unsigned int score_1 = 0;
    unsigned int score_2 = 0;
    bool started = false;
    

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        if (started) {
            auto dt = clock.restart().asSeconds();
            compute_velocities(dt, rectangles, ball);
            make_move(dt, rectangles, ball);
            // resolve_collisions(rectangles, net, ball, walls);
            do_rectangle_collisions(rectangles, net);
            do_ball_collisions(ball, net, rectangles, score_1, score_2);
        } else {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter)) {
                clock.reset();
                started = true;
                score_1 = 0;
                score_2 = 0;
            }
        }

        if (score_1 == 10 || score_2 == 10) {
            started = false;
            main_text.setString(std::format("Player {} won! Press ENTER to restart", score_1 > score_2 ? "1" : "2"));
        }


        text_1.setString(std::format("Player 1: {}", score_1));
        text_2.setString(std::format("Player 2: {}", score_2));
        window.clear(sf::Color::Black);

        for (const auto& rectangle : rectangles) {
            window.draw(rectangle.shape);
        }

        window.draw(line);
        window.draw(net);
        window.draw(ball.shape);
        window.draw(text_1);
        window.draw(text_2);

        if (!started) {
            window.draw(main_text);
        }

        window.display();
    }

    return 0;
}