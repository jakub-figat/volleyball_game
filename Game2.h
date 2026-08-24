#pragma once

#include <random>

#include "SFML/Graphics.hpp"
#include "constants.cpp"

struct Rectangle {
  sf::RectangleShape shape;
  float velocity_x, velocity_y;

  sf::Vector2f get_max_position() const { return shape.getPosition() + shape.getSize(); }
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

enum class GameState { STARTING, RUNNING, PAUSED, FINISHED };

class Game {
 public:
  Game()
      : window(sf::VideoMode({static_cast<int>(WIDTH), static_cast<int>(HEIGHT)}), "vgame", sf::Style::Close),
        font("Pixel Game.otf"),
        text_1(font),
        text_2(font),
        main_text(font),
        rectangle_shape({20.f, 60.f}),
        rectangles({{sf::RectangleShape{rectangle_shape}, PADDLE_SPEED, 0.0},
                    {sf::RectangleShape{rectangle_shape}, PADDLE_SPEED, 0.0}}),
        ball({sf::RectangleShape{{BALL_SIZE, BALL_SIZE}}, 0.0, 0.0}),
        walls({
            sf::RectangleShape{{100.0f, HEIGHT}},  // left
            sf::RectangleShape{{100.0f, HEIGHT}},  // right
            sf::RectangleShape{{WIDTH, 100.0f}},   // ceiling
            sf::RectangleShape{{WIDTH, 100.0f}},   // bottom
            sf::RectangleShape{{10.0f, 150.0f}},   // net
            sf::RectangleShape{{WIDTH, 2.0f}}      // floor
        }),
        state(GameState::STARTING) {

    text_1.setCharacterSize(45);
    text_2.setCharacterSize(45);
    text_1.setString("Player 1: 0");
    text_2.setString("Player 2: 0");

    main_text.setCharacterSize(45);

    main_text.setString("Press ENTER to start");

    text_1.setFillColor(sf::Color::White);
    text_2.setFillColor(sf::Color::White);
    main_text.setFillColor(sf::Color::White);

    text_1.move({30.0, 30.0});
    text_2.move({30.0, 60.0});
    main_text.move({30.0f, 90.0f});

    rectangles[0].shape.setPosition({MIDDLE - 200, FLOOR_HEIGHT - rectangle_shape.y});
    rectangles[1].shape.setPosition({MIDDLE + 200, FLOOR_HEIGHT - rectangle_shape.y});

    rectangles[0].shape.setFillColor(sf::Color::White);
    rectangles[1].shape.setFillColor(sf::Color::White);

    ball.shape.setFillColor(sf::Color::White);
    ball.shape.setPosition(BALL_START_POSITION);
    ball.serve();

    walls[0].setPosition({-100.0f, 0.0f});
    walls[1].setPosition({WIDTH, 0.0});
    walls[2].setPosition({0.0f, -100.0f});
    walls[3].setPosition({0.0, HEIGHT});
    walls[4].setPosition({MIDDLE - 5.0f, FLOOR_HEIGHT - walls[4].getSize().y});

    walls[4].setFillColor(sf::Color::White);

    walls[5].setPosition({0.0f, FLOOR_HEIGHT});
    walls[5].setFillColor(sf::Color::White);

    window.setFramerateLimit(120);
  }

  void start() {
    while (window.isOpen()) {
      while (const auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
          window.close();
        }

        process_events(event.value());
      }

      auto dt = clock.restart().asSeconds();
      if (state == GameState::RUNNING) {
        update(dt);
      }
      render();
    }
  }

 private:
  sf::RenderWindow window;
  sf::Font font;
  sf::Text text_1, text_2, main_text;

  sf::Vector2f rectangle_shape;
  std::vector<Rectangle> rectangles;
  Ball ball;
  std::vector<sf::RectangleShape> walls;

  sf::Clock clock;
  unsigned int score_1 = 0, score_2 = 0;
  GameState state;

 private:
  void process_events(sf::Event event) {
    if (auto key = event.getIf<sf::Event::KeyPressed>()) {
      if (key->code == sf::Keyboard::Key::Enter && state != GameState::RUNNING) {
        clock.restart();
        main_text.setString("Press ESC to pause");

        if (state == GameState::FINISHED) {
          ball.serve();
          rectangles[0].shape.setPosition({MIDDLE - 200, FLOOR_HEIGHT - rectangle_shape.y});
          rectangles[1].shape.setPosition({MIDDLE + 200, FLOOR_HEIGHT - rectangle_shape.y});

          score_1 = 0;
          score_2 = 0;
        }
        state = GameState::RUNNING;
      }

      if (key->code == sf::Keyboard::Key::Escape && state == GameState::RUNNING) {
        state = GameState::PAUSED;
        main_text.setString("Press Enter to resume");
      }
    }
  }
  void update(float dt) {
    compute_velocities(dt);
    make_move(dt);
    resolve_collisions();

    text_1.setString(std::format("Player 1: {}", score_1));
    text_2.setString(std::format("Player 2: {}", score_2));
    if (score_1 == MAX_SCORE || score_2 == MAX_SCORE) {
      state = GameState::FINISHED;
      main_text.setString(std::format("Player {} won! Press ENTER to restart", score_1 > score_2 ? "1" : "2"));
    }
  }

  void render() {
    window.clear();

    if (state != GameState::STARTING) {
      window.draw(text_1);
      window.draw(text_2);
    }
    window.draw(main_text);

    window.draw(rectangles[0].shape);
    window.draw(rectangles[1].shape);

    window.draw(ball.shape);

    window.draw(walls[4]);
    window.draw(walls[5]);

    window.display();
  }

  void compute_velocities(float dt) {
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

  void make_move(float dt) {
    for (auto& rectangle : rectangles) {
      rectangle.shape.move({rectangle.velocity_x * dt, rectangle.velocity_y * dt});
    }
    ball.shape.move({ball.velocity_x * dt, ball.velocity_y * dt});
  }

  static std::optional<Overlap> get_overlap(const sf::RectangleShape& rectangle, const sf::RectangleShape& other) {
    auto [rectangle_x, rectangle_y] = rectangle.getPosition();
    auto [rectangle_max_x, rectangle_max_y] = rectangle.getPosition() + rectangle.getSize();

    auto [other_x, other_y] = other.getPosition();
    auto [other_max_x, other_max_y] = other.getPosition() + other.getSize();

    float overlap_x = std::min(rectangle_max_x, other_max_x) - std::max(rectangle_x, other_x);
    float overlap_y = std::min(rectangle_max_y, other_max_y) - std::max(rectangle_y, other_y);

    if (overlap_x <= 0 || overlap_y <= 0) {
      return std::nullopt;
    }

    return Overlap{overlap_x, overlap_y};
  }

  void resolve_rectangles_with_solids() {
    for (auto& rectangle : rectangles) {
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

  void resolve_ball_with_solids() {
    for (const auto& wall : walls) {
      if (auto overlap = get_overlap(ball.shape, wall)) {
        if (overlap->x > overlap->y) {
          // special check: floor
          if (&wall == &walls[5]) {
            (ball.shape.getPosition().x > MIDDLE ? score_1 : score_2) += 1;
            ball.serve();
          } else {
            auto sign = ball.shape.getPosition().y < wall.getPosition().y ? -1.0f : 1.0f;

            ball.shape.move({0.0f, sign * overlap->y});
            ball.velocity_y = -ball.velocity_y;
          }
        } else {
          auto sign = ball.shape.getPosition().x < wall.getPosition().x ? -1.0f : 1.0f;

          ball.shape.move({sign * overlap->x, 0.0f});
          ball.velocity_x = -ball.velocity_x;
        }
      }
    }
  }

  void resolve_ball_with_rectangles() {
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

  void resolve_collisions() {
    resolve_rectangles_with_solids();

    resolve_ball_with_solids();
    resolve_ball_with_rectangles();
  }
};