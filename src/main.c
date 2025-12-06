#include <raylib.h>
#include "car.h"
#include "ui.h"
#include "map.h"
#include "textures.h"
#include "audio.h"

typedef enum {MENU, GAME_SETUP, PLAYING, VICTORY, PAUSE, COUNTDOWN} State;




int main(){
   
    int initialPosX, initialPosY;
    const int virtualScreenWidth = 1920;
    const int virtualScreenHeight = 1080;

    
    MAP gameMap = loadMap("track1.txt");
    getInitialPosition(&gameMap, &initialPosX, &initialPosY);

    InitWindow(0, 0, "Rock N' Roll Racing");
    SetWindowState(FLAG_FULLSCREEN_MODE);
    SetExitKey(0);
    InitAudioDevice();
    GAME_TEXTURES gameTextures = loadTextures();
    CAR player = {initialPosX,initialPosY,0.0f,60.0f,28.0f,gameTextures.carTextures[0],0.0f,420.0f,250.0f,300.0f,200.0f,0,0,0,(float)initialPosX,(float)initialPosY,100,0};
    GameAudio gameAudio = LoadGameAudio();
    
    Rectangle menuButtons[3] = {{798,600,385,60},{798,670,385,60},{798,740,160,60}};
    Rectangle pauseMenuButtons[3] = {{785,510,345,60},{715,600,480,60},{715,690,480,50}};

    const char* carNames[] = { "RED RACER", "BLUE CRUISER", "THE TANK", "STAR FIGHTER" };
    int totalCars = 4;
    int selectedCarIndex = 0;

    const char* mapFiles[] = { "track1.txt", "track1.txt" }; 
    const char* mapNames[] = { "Track 1", "Track 2" };
    int totalMaps = 2;
    int selectedMapIndex = 0;

    int numLaps = 3;
    int numEnemies = 1;
    float countdownTimer = 0.0f;

    // Retângulos da Tela de Setup (Botões < e > e Start/Back)
    // Layout: Carro (Topo), Mapa, Voltas, Inimigos, Start
    Rectangle setupBtnBack = { 50, 50, 150, 50 };
    Rectangle setupBtnStart = { 760, 900, 400, 80 };
    
    // Botões de navegação [<] [>] para cada opção
    // Ordem: Carro, Mapa, Voltas, Inimigos
    Rectangle btnPrevCar = { 600, 300, 50, 50 };
    Rectangle btnNextCar = { 1270, 300, 50, 50 };
    
    Rectangle btnPrevMap = { 600, 500, 50, 50 };
    Rectangle btnNextMap = { 1270, 500, 50, 50 };

    Rectangle btnPrevLap = { 600, 600, 50, 50 };
    Rectangle btnNextLap = { 1270, 600, 50, 50 };

    Rectangle btnPrevEnemy = { 600, 700, 50, 50 };
    Rectangle btnNextEnemy = { 1270, 700, 50, 50 };
    
    State currentState = MENU;
    
    RenderTexture2D screen = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    RENDER_SETUP screenSetup = {screen, virtualScreenWidth, virtualScreenHeight, {0}, {0}};
    screenSetup = calculateScreenSetup(screenSetup);
    
    Camera2D camera = {0};
    camera.offset = (Vector2){ virtualScreenWidth / 2.0f, virtualScreenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.4f; 
    
    while(!WindowShouldClose()){
        
        Vector2 mousePos = GetMousePosition();
        UpdateGameMusic(&gameAudio, (int)currentState);
        
        if (currentState == PLAYING) {
            updateCar(&player, &gameMap);
            updateLaps(&player, &gameMap);
            camera.target = (Vector2){ player.x, player.y };
        }
        BeginTextureMode(screen);
            switch (currentState){
                case MENU:{
                    ClearBackground(BLACK);
                    DrawTextEx(gameTextures.titlefont, "Rock N' Roll", (Vector2){190,210},200,10,RAYWHITE);
                    DrawTextEx(gameTextures.titlefont, "Racing", (Vector2){505,350},200,10,RAYWHITE);
                    DrawTextEx(gameTextures.titlefont, "New Game", (Vector2){798,600},50,10,RAYWHITE);
                    DrawTextEx(gameTextures.titlefont, "Continue", (Vector2){798,670},50,10,RAYWHITE);
                    DrawTextEx(gameTextures.titlefont, "Exit", (Vector2){798,740},50,10,RAYWHITE);
                    if(CheckCollisionPointRec(GetMousePosition(),menuButtons[0])){
                        DrawTextEx(gameTextures.titlefont, "New Game", (Vector2){798,600},50,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                            currentState = GAME_SETUP;
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
                case GAME_SETUP:{
                    ClearBackground(BLACK);
                    DrawTextEx(gameTextures.titlefont, "Choose your setup", (Vector2){460, 100}, 80, 5, WHITE);

                    Texture2D currentCarTex = gameTextures.carTextures[selectedCarIndex];
                    
                    DrawTexturePro(currentCarTex,(Rectangle){0, 0, currentCarTex.width, currentCarTex.height},(Rectangle){860, 250, 200, 100},(Vector2){0,0}, 0.0f, WHITE);                                      
                    
                    // Botões Carro < >
                    DrawText("<", btnPrevCar.x, btnPrevCar.y, 50, CheckCollisionPointRec(mousePos, btnPrevCar) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(mousePos, btnPrevCar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedCarIndex--; if(selectedCarIndex < 0) selectedCarIndex = totalCars - 1;
                    }
                    DrawText(">", btnNextCar.x, btnNextCar.y, 50, CheckCollisionPointRec(mousePos, btnNextCar) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(mousePos, btnNextCar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedCarIndex = (selectedCarIndex + 1) % totalCars;
                    }

                    // SELEÇÃO DE MAPA 
                    DrawTextEx(gameTextures.titlefont, TextFormat("Map: %s", mapNames[selectedMapIndex]), (Vector2){790, 500}, 40, 2, WHITE);
                    DrawText("<", btnPrevMap.x, btnPrevMap.y, 50, CheckCollisionPointRec(mousePos, btnPrevMap) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(mousePos, btnPrevMap) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedMapIndex--; if(selectedMapIndex < 0) selectedMapIndex = totalMaps - 1;
                    }
                    DrawText(">", btnNextMap.x, btnNextMap.y, 50, CheckCollisionPointRec(mousePos, btnNextMap) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(mousePos, btnNextMap) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedMapIndex = (selectedMapIndex + 1) % totalMaps;
                    }

                    // VOLTAS 
                    DrawTextEx(gameTextures.titlefont, TextFormat("Laps: %02d", numLaps), (Vector2){850, 600}, 40, 2, WHITE);
                    DrawText("<", btnPrevLap.x, btnPrevLap.y, 50, CheckCollisionPointRec(mousePos, btnPrevLap) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(mousePos, btnPrevLap) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(numLaps > 1) numLaps--;
                    }
                    DrawText(">", btnNextLap.x, btnNextLap.y, 50, CheckCollisionPointRec(mousePos, btnNextLap) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(mousePos, btnNextLap) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(numLaps < 50) numLaps++;
                    }

                    // INIMIGOS 
                    DrawTextEx(gameTextures.titlefont, TextFormat("Enemies: %d", numEnemies), (Vector2){830, 700}, 40, 2, WHITE);
                    DrawText("<", btnPrevEnemy.x, btnPrevEnemy.y, 50, CheckCollisionPointRec(mousePos, btnPrevEnemy) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(mousePos, btnPrevEnemy) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(numEnemies > 0) numEnemies--;
                    }
                    DrawText(">", btnNextEnemy.x, btnNextEnemy.y, 50, CheckCollisionPointRec(mousePos, btnNextEnemy) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(mousePos, btnNextEnemy) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(numEnemies < 5) numEnemies++;
                    }
                    // BOTÃO START 
                    Color startColor = CheckCollisionPointRec(mousePos, setupBtnStart) ? YELLOW : GREEN;
                    DrawRectangleLinesEx(setupBtnStart, 5, startColor);
                    DrawTextEx(gameTextures.titlefont, "START RACE", (Vector2){setupBtnStart.x+ 10, setupBtnStart.y + 15}, 48, 5, startColor);

                    if(CheckCollisionPointRec(mousePos, setupBtnStart) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    // INICIA O JOGO COM AS CONFIGURAÇÕES
                    gameMap = loadMap(mapFiles[selectedMapIndex]);
                    getInitialPosition(&gameMap, &initialPosX, &initialPosY);
                        
                    // Reseta o player na posição correta do novo mapa
                    resetCar(&player,initialPosX,initialPosY);
                    player.maxLaps = numLaps;
                    player.carTexture = gameTextures.carTextures[selectedCarIndex];
                    camera.target = (Vector2){ player.x, player.y };
                    countdownTimer = 4.0f; // 3 segundos de contagem + 1 segundo para o "GO!"
                    PlaySound(gameAudio.countdown);
                    currentState = COUNTDOWN;
                }
                    // BOTÃO VOLTAR 
                    DrawTextEx(gameTextures.titlefont, "BACK", (Vector2){setupBtnBack.x, setupBtnBack.y}, 30, 2, CheckCollisionPointRec(mousePos, setupBtnBack)?RED:WHITE);
                    if(CheckCollisionPointRec(mousePos, setupBtnBack) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        currentState = MENU;
                    }
                        break;
                    }
                case COUNTDOWN: {
                    countdownTimer -= GetFrameTime();

                    

                    // 3. Transição para o Jogo quando o tempo acaba
                    if (countdownTimer <= 0.0f) {
                        currentState = PLAYING;
                    }
                    ClearBackground(DARKGREEN);
                    BeginMode2D(camera);
                        drawMap(&gameMap, &gameTextures);
                        drawCar(&player); // Desenha carro parado
                    EndMode2D();
                    
                    // Desenha HUD estático (opcional)
                    
                    // Desenha Números do Countdown no Centro
                    const char* text;
                    Color color = RED;
                    int fontSize = 300;

                    if (countdownTimer > 1.0f) {
                        // Mostra 3, 2, 1 (subtrai 1 para ajustar a escala de 4s para 3s visuais)
                        text = TextFormat("%d", (int)countdownTimer);
                        color = (countdownTimer > 2.0f) ? RED : ORANGE;
                    } else {
                        text = "GO!";
                        color = GREEN;
                    }

                    // Centraliza o texto
                    Vector2 textSize = MeasureTextEx(gameTextures.titlefont, text, fontSize, 10);
                    DrawTextEx(gameTextures.titlefont, text,(Vector2){(virtualScreenWidth - textSize.x)/2, (virtualScreenHeight - textSize.y)/2},fontSize, 10, color);
                    break;
                }
                case PLAYING:{
                    ClearBackground(DARKGREEN); 
                    BeginMode2D(camera);
                        drawMap(&gameMap, &gameTextures);
                        drawCar(&player);
                    EndMode2D();
                    drawHUD(&player, &player, &gameTextures, virtualScreenWidth, virtualScreenHeight);
                    DrawFPS(virtualScreenWidth - 100, 0);
                    if(IsKeyDown(KEY_ESCAPE))
                        currentState = PAUSE;
                    if(player.currentLap == numLaps)
                        currentState = VICTORY;
                    break;
                }
                case PAUSE:{
                    ClearBackground(BLACK);
                    DrawTextEx(gameTextures.titlefont, "Paused", (Vector2){640,210},130,10,RAYWHITE);
                    DrawTextEx(gameTextures.titlefont, "Resume", (Vector2){785,510},60,10,RAYWHITE);
                    DrawTextEx(gameTextures.titlefont, "Save game", (Vector2){715,600},60,10,RAYWHITE);
                    DrawTextEx(gameTextures.titlefont, "Main menu", (Vector2){715,690},60,10,RAYWHITE);
                    if(CheckCollisionPointRec(GetMousePosition(),pauseMenuButtons[0])){
                        DrawTextEx(gameTextures.titlefont, "Resume", (Vector2){785,510},60,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                            currentState = PLAYING;
                    }
                    else if(CheckCollisionPointRec(GetMousePosition(),pauseMenuButtons[1])){
                        DrawTextEx(gameTextures.titlefont, "Save game", (Vector2){715,600},60,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                            currentState = MENU;
                    }
                    else if(CheckCollisionPointRec(GetMousePosition(),pauseMenuButtons[2])){
                        DrawTextEx(gameTextures.titlefont, "Main menu", (Vector2){715,690},60,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                            currentState = MENU;
                    }
                    break;
                }
                case VICTORY:{
                    DrawTextEx(gameTextures.titlefont,"* VICTORY *",(Vector2) {(virtualScreenWidth / 2) - 674, 340},200,5,GOLD);
                    break;
                }    
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
    UnloadGameAudio(&gameAudio);
    CloseAudioDevice();
    CloseWindow();        

    return 0;
}