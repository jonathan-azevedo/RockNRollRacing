#include <raylib.h>
#include "car.h"
#include "ui.h"


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
    while (!WindowShouldClose())    
    {
        updateCar(&player);
        BeginTextureMode(screen);
            ClearBackground(RAYWHITE); 
            drawCar(&player);
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