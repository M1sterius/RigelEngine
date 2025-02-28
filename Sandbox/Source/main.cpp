#include <numbers>

#include "RigelEngine.hpp"
#include "TestComponent.hpp"

class Game
{
public:
    Game() = default;
    ~Game() = default;

    void Init()
    {
        m_Engine = rge::Engine::CreateInstance();
        rge::Time::SetTargetFPS(30);
    }

    void Run()
    {
        m_Engine->Run();
    }
private:
    std::unique_ptr<rge::Engine> m_Engine;
};

int32_t main(int32_t argc, char* argv[])
{
    Game game;
    game.Init();
    game.Run();
}
