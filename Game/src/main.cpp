#include <Engine.hpp>
#include <GLFW/glfw3.h>
#include <random>

constexpr int WIDTH = 32, HEIGHT = 18;

constexpr glm::vec4 SNAKE_COLOR = {0.09, 0.671, 0, 1};
constexpr glm::vec4 APPLE_COLOR = {0.98, 0.216, 0, 1};

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

        snake.push_back({WIDTH/2, HEIGHT/2});
        snake.push_back({WIDTH/2, HEIGHT/2 + 1});

        rollApple();
    }

    void run(double dt, uint32_t currentFrame, fly::Engine& engine) override {
        assert(dt >= 0);
        (void) currentFrame;

        if(dead) {
            //std::cout << "DEAD" << std::endl;
            engine.getTextRenderer().renderText("DS_DIGITAL", "GAME OVER", {1280/2, 720/2 - 200}, fly::Align::CENTER, 200, {1, 1, 1, 1});
            
            if(engine.getWindow().keyJustPressed(GLFW_KEY_ENTER))
                dead = false;
            
            return;
        }
        
        this->ts += dt;
        
        bool touched = false;
        if(engine.getWindow().keyJustPressed(GLFW_KEY_LEFT) && dir != 1) {
            dir = 3;
            touched = true;
        } else if(engine.getWindow().keyJustPressed(GLFW_KEY_RIGHT) && dir != 3) {
            dir = 1;
            touched = true;
        }

        if(engine.getWindow().keyJustPressed(GLFW_KEY_UP) && dir != 2) {
            dir = 0;
            touched = true;
        } else if(engine.getWindow().keyJustPressed(GLFW_KEY_DOWN) && dir != 0) {
            dir = 2;
            touched = true;
        }

        if(this->ts >= this->threshold || touched) {
            this->ts = 0;

            glm::ivec2 newHead = snake.front() + glm::ivec2(incX[dir], incY[dir]);
            if(0 <= newHead.x && newHead.x < WIDTH && 0 <= newHead.y && newHead.y < HEIGHT 
            && std::find(snake.begin(), snake.end(), newHead) == snake.end()) {
                snake.push_front(newHead);

                if(newHead == apple) {
                    rollApple();
                    this->threshold -= 0.05;
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

        /*for(int i=0; i<WIDTH; ++i) {
            for(int j=0; j<HEIGHT; ++j) {
                if(std::find(snake.begin(), snake.end(), glm::ivec2{i, j}) != snake.end() || std::find(snake.begin(), snake.end(), apple) != snake.end())
                    continue;

                auto color = (i+j)%2? glm::vec4{0.588, 0.761, 0.529, 1} : glm::vec4{0.427, 0.741, 0.322, 1};
                engine.getRenderer2d().renderQuad({tileWidth * i, tileHeight * j}, {tileWidth, tileHeight}, color);
            }
        }*/
    }

    void rollApple() {
        glm::ivec2 pos;
        
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<int> distX(0,WIDTH-1), distY(0, HEIGHT-1); 
        
        do {
            pos.x = distX(rng);
            pos.y = distY(rng);
        } while(std::find(snake.begin(), snake.end(), pos) != snake.end() || pos == apple);

        apple = pos;
    }

private:
    double ts = 0;
    std::deque<glm::ivec2> snake;
    int incX[4] = {0, 1, 0, -1};
    int incY[4] = {-1, 0, 1, 0};
    int dir = 0;
    glm::ivec2 apple;
    bool dead = false;
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