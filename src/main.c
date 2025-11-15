#include <raylib.h>
#include "car.h"
#include "ui.h"
#include "map.h"


int main()
{
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);
    const int virtualScreenWidth = 1920;
    const int virtualScreenHeight = 1080;
    CAR player = {20.0f,20.0f,0.0f,64.0f,32.0f};

    InitWindow(screenWidth, screenHeight, "Rock and Roll Racing");
    ToggleFullscreen();
    
    RenderTexture2D screen = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    RENDER_SETUP screenSetup = {screen, virtualScreenWidth, virtualScreenHeight, {0}, {0}};
    screenSetup = calculateScreenSetup(screenSetup);
    
    Camera2D camera = { 0 };
    camera.offset = (Vector2){ virtualScreenWidth / 2.0f, virtualScreenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.4f; 

    MAP gameMap = loadMap("track1.txt");
    while (!WindowShouldClose())    
    {
        updateCar(&player);
        camera.target = (Vector2){ player.x, player.y };
        BeginTextureMode(screen);
            ClearBackground(DARKGREEN); 
            BeginMode2D(camera);
                drawMap(&gameMap);
                drawCar(&player);
            EndMode2D();
            DrawFPS(virtualScreenWidth - 100, 0);
        EndTextureMode();
        BeginDrawing();
            ClearBackground(BLACK); 
            DrawTexturePro(screen.texture, screenSetup.sourceRec, screenSetup.destRec, (Vector2){0, 0}, 0.0f, WHITE);
        EndDrawing();
        }
    UnloadRenderTexture(screen);
    CloseWindow();        

    return 0;
}