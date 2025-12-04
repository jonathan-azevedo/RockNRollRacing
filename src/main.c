#include <raylib.h>
#include "car.h"
#include "ui.h"
#include "map.h"
#include "textures.h"

typedef enum {MENU, NEWGAME_SELECTION, PLAYING} State;




int main(){
   
    int initialPosX, initialPosY;
    const int virtualScreenWidth = 1920;
    const int virtualScreenHeight = 1080;
    Rectangle menuButtons[3] = {{798,600,385,40},{798,670,385,40},{798,740,160,40}};

    State currentState = MENU;
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
            switch (currentState){
                    case MENU:{
                        ClearBackground(BLACK);
                        DrawTextEx(gameTextures.titlefont, "Rock and Race", (Vector2){190,300},160,10,RAYWHITE);
                        DrawTextEx(gameTextures.titlefont, "New Game", (Vector2){798,600},50,10,RAYWHITE);
                        DrawTextEx(gameTextures.titlefont, "Continue", (Vector2){798,670},50,10,RAYWHITE);
                        DrawTextEx(gameTextures.titlefont, "Exit", (Vector2){798,740},50,10,RAYWHITE);
                        if(CheckCollisionPointRec(GetMousePosition(),menuButtons[0])){
                            DrawTextEx(gameTextures.titlefont, "New Game", (Vector2){798,600},50,10,LIGHTGRAY);
                            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                                currentState = NEWGAME_SELECTION;
                        }
                        else if(CheckCollisionPointRec(GetMousePosition(),menuButtons[1])){
                            DrawTextEx(gameTextures.titlefont, "Continue", (Vector2){798,670},50,10,LIGHTGRAY);
                            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                                currentState = PLAYING;
                        }
                        else if(CheckCollisionPointRec(GetMousePosition(),menuButtons[2])){
                            DrawTextEx(gameTextures.titlefont, "Exit", (Vector2){798,740},50,10,LIGHTGRAY);
                            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                                goto endgame;
                        }
                        break;
                    }
                    case NEWGAME_SELECTION:{
                        ClearBackground(BLACK);



                        break;
                    }
                    case PLAYING:{
                        ClearBackground(DARKGREEN); 
                        BeginMode2D(camera);
                            drawMap(&gameMap, &gameTextures);
                            drawCar(&player);
                        EndMode2D();
                        DrawFPS(virtualScreenWidth - 100, 0);
                        DrawText(TextFormat("Vel: %.0f", player.currentSpeed), 10, 10, 20, WHITE);
                        DrawText(TextFormat("Ang: %.0f", player.angle), 10, 40, 20, WHITE);
                        DrawText(TextFormat("Volta: %d", player.currentLap + 1), 50, 20, 40, YELLOW);
                    }
                        break;
                    }

        
        EndTextureMode();
        BeginDrawing();
            ClearBackground(BLACK); 
            DrawTexturePro(screen.texture, screenSetup.sourceRec, screenSetup.destRec, (Vector2){0, 0}, 0.0f, WHITE);
        EndDrawing();
        }
    endgame:
    UnloadRenderTexture(screen);
    unloadTextures(&gameTextures);
    CloseWindow();        

    return 0;
}