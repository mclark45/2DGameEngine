#include "./Game/Game.hpp"

int main(int argc, char* args[]) {
    // create game object
    Game game(1000, 800);

    // start sdl and create the window and renderer
    game.Initialize();
    // run the main program loop
    game.Run();
    // free allocated resources and quit sdl
    game.Destroy();

    return 0;
}