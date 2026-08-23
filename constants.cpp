#include <SFML/Graphics.hpp>

constexpr float WIDTH = 1000.0;
constexpr float HEIGHT = 800.0;
constexpr float MIDDLE = WIDTH / 2;
constexpr float FLOOR_HEIGHT = 600.0;

constexpr float BALL_SIZE = 20.0;
constexpr sf::Vector2f BALL_START_POSITION {MIDDLE - BALL_SIZE / 2.0f, 200.0};

constexpr float GRAVITY = 300.0;
constexpr float JUMP_VELOCTIY = -250.0;
constexpr float PADDLE_SPEED = 400.0;
constexpr float HIT_FORCE = 1.5;