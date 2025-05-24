#include <Engine.hpp>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <random>

constexpr int WIDTH = 32, HEIGHT = 18;

constexpr glm::vec4 SNAKE_COLOR = {0.09, 0.471, 0, 1};
constexpr glm::vec4 APPLE_COLOR = {0.98, 0.116, 0, 1};

class SnakeGame: public fly::Scene {
public:
    SnakeGame() = default;
    ~SnakeGame() = default;

    void init(fly::Engine& engine) override {
        engine.getTextRenderer().loadFont(
            "DS_DIGITAL", 
            std::filesystem::path("assets/font.png"), 
            std::filesystem::path("assets/font.json")
        );

        distX = std::uniform_int_distribution(0, WIDTH-1);
        distY = std::uniform_int_distribution(0, HEIGHT-1);

        start();
    }

    void start() {
        dead = false;
        newDir = dir = 0;
        this->threshold = 0.4;

        snake.clear();
        snake.push_back({WIDTH/2, HEIGHT/2});
        snake.push_back({WIDTH/2, HEIGHT/2 + 1});
        
        rollApple();
    }

    void run(double dt, uint32_t currentFrame, fly::Engine& engine) override {
        assert(dt >= 0);
        (void) currentFrame;

        if(dead) {
            //std::cout << "DEAD" << std::endl;
            engine.getTextRenderer().renderText("DS_DIGITAL", "GAME OVER", {1280/2, 720/2 - 150}, fly::Align::CENTER, 200, {1, 1, 1, 1});
            
            if(engine.getWindow().keyJustPressed(GLFW_KEY_ENTER)) {
                start();
            }
            
            return;
        }
        
        this->ts += dt;
        
        if(engine.getWindow().keyJustPressed(GLFW_KEY_LEFT) && dir != 1) {
            newDir = 3;
        } else if(engine.getWindow().keyJustPressed(GLFW_KEY_RIGHT) && dir != 3) {
            newDir = 1;
        }

        if(engine.getWindow().keyJustPressed(GLFW_KEY_UP) && dir != 2) {
            newDir = 0;
        } else if(engine.getWindow().keyJustPressed(GLFW_KEY_DOWN) && dir != 0) {
            newDir = 2;
        }

        float completion = this->ts / this->threshold;

        if(completion >= 1) {
            dir = newDir;
            this->ts = 0;

            glm::ivec2 newHead = snake.front() + glm::ivec2(incX[dir], incY[dir]);
            if(0 <= newHead.x && newHead.x < WIDTH && 0 <= newHead.y && newHead.y < HEIGHT 
            && std::find(snake.begin(), snake.end(), newHead) == snake.end()) {
                snake.push_front(newHead);

                if(newHead == apple) {
                    rollApple();
                    this->threshold -= 0.01;
                } else {
                    snake.pop_back();
                }
            } else {
                dead = true;
            }
        }

        auto tileWidth = (float) engine.getWindow().getWidth() / WIDTH;
        auto tileHeight = (float) engine.getWindow().getHeight() / HEIGHT;
        for(auto e: snake) {
            engine.getRenderer2d().renderQuad({tileWidth * e.x, tileHeight * e.y}, {tileWidth, tileHeight}, SNAKE_COLOR);
        }

        engine.getRenderer2d().renderQuad({tileWidth * apple.x, tileHeight * apple.y}, {tileWidth, tileHeight}, APPLE_COLOR);

        for(int i=0; i<WIDTH; ++i) {
            for(int j=0; j<HEIGHT; ++j) {
                if(std::find(snake.begin(), snake.end(), glm::ivec2{i, j}) != snake.end() || glm::ivec2{i,j} == apple)
                    continue;

                auto color = (i+j)%2? glm::vec4{0.488, 0.661, 0.429, 1} : glm::vec4{0.327, 0.641, 0.222, 1};
                engine.getRenderer2d().renderQuad({tileWidth * i, tileHeight * j}, {tileWidth, tileHeight}, color);
            }
        }

        ImGui::LabelText("Pos:", "%d,%d", snake.front().x, snake.front().y);
    }

    void rollApple() {
        glm::ivec2 pos;
        
        do {
            pos.x = distX(rd);
            pos.y = distY(rd);
            //std::cout << "Dice: " << pos.x << ' ' << pos.y << std::endl;
        } while(std::find(snake.begin(), snake.end(), pos) != snake.end());

        apple = pos;
    }

private:
    std::default_random_engine rd{std::random_device()()};
    std::uniform_int_distribution<int> distX, distY;
    double ts = 0;
    std::deque<glm::ivec2> snake;
    int incX[4] = {0, 1, 0, -1};
    int incY[4] = {-1, 0, 1, 0};
    int dir = 0, newDir = 0;
    glm::ivec2 apple;
    bool dead;
    float threshold = 0.4;

};


int main() {
    fly::Engine engine(1280, 720);

    try {
        engine.init();

        engine.setScene<SnakeGame>();

        engine.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}