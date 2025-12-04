#include <raylib.h>
#include "car.h"
#include "ui.h"
#include "map.h"
#include "textures.h"

int main(){
   
    int initialPosX, initialPosY;
    const int virtualScreenWidth = 1920;
    const int virtualScreenHeight = 1080;

    
    MAP gameMap = loadMap("track1.txt");
    getInitialPosition(&gameMap, &initialPosX, &initialPosY);

    InitWindow(0, 0, "Rock and Roll Racing");
    SetWindowState(FLAG_FULLSCREEN_MODE);
    GAME_TEXTURES gameTextures = loadTextures();
    CAR player = {initialPosX,initialPosY,0.0f,60.0f,28.0f,gameTextures.carTexture,0.0f,420.0f,250.0f,300.0f,200.0f,0,0,(float)initialPosX,(float)initialPosY};
    
    RenderTexture2D screen = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    RENDER_SETUP screenSetup = {screen, virtualScreenWidth, virtualScreenHeight, {0}, {0}};
    screenSetup = calculateScreenSetup(screenSetup);
    
    Camera2D camera = {0};
    camera.offset = (Vector2){ virtualScreenWidth / 2.0f, virtualScreenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.4f; 

    
    while(!WindowShouldClose()){
        updateCar(&player, &gameMap);
        updateLaps(&player, &gameMap);
        camera.target = (Vector2){ player.x, player.y };
        BeginTextureMode(screen);
            ClearBackground(DARKGREEN); 
            BeginMode2D(camera);
                drawMap(&gameMap, &gameTextures);
                drawCar(&player);
            EndMode2D();
            DrawFPS(virtualScreenWidth - 100, 0);
            DrawText(TextFormat("Vel: %.0f", player.currentSpeed), 10, 10, 20, WHITE);
            DrawText(TextFormat("Ang: %.0f", player.angle), 10, 40, 20, WHITE);
            DrawText(TextFormat("Volta: %d", player.currentLap + 1), 50, 20, 40, YELLOW);
        EndTextureMode();
        BeginDrawing();
            ClearBackground(BLACK); 
            DrawTexturePro(screen.texture, screenSetup.sourceRec, screenSetup.destRec, (Vector2){0, 0}, 0.0f, WHITE);
        EndDrawing();
        }
    UnloadRenderTexture(screen);
    unloadTextures(&gameTextures);
    CloseWindow();        

    return 0;
}