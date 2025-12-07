#include <raylib.h>
#include "car.h"
#include "ui.h"
#include "map.h"
#include "textures.h"
#include "audio.h"
#include "itens.h"
#include "enemy.h"
#include <stdio.h>

#define MAX_ENEMIES 2

// Estrutura apenas para guardar dados na memória RAM (temporário)
typedef struct {
    int unlockedCars;
    int highScore;
} SAVE_DATA;

typedef enum {MENU, GAME_SETUP, PLAYING, VICTORY, PAUSE, COUNTDOWN} State;

int main(){
    const int virtualScreenWidth = 1920;
    const int virtualScreenHeight = 1080;

    InitWindow(0, 0, "Rock N' Roll Racing");
    SetWindowState(FLAG_FULLSCREEN_MODE);
    SetExitKey(0);
    InitAudioDevice();

    GAME_TEXTURES gameTextures = loadTextures();
    GameAudio gameAudio = LoadGameAudio();

    // --- ALTERADO: INICIA SEMPRE DO ZERO ---
    // {1, 0} significa: 1 carro liberado, 0 vitórias.
    SAVE_DATA saveData = {1, 0};

    MAP gameMap = {0};
    int initialPosX, initialPosY;

    CAR player = {0};
    ENEMY enemies[MAX_ENEMIES] = {0};
    ITEM_MANAGER itemManager = {0};

    bool playerWon = false;
    bool isGameRunning = false;
    int winningEnemyIndex = -1;

    Rectangle menuButtons[3] = {{798,600,385,60},{798,670,385,60},{798,740,160,60}};
    Rectangle pauseMenuButtons[3] = {{785,510,345,60},{715,600,480,60},{715,690,480,50}};

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

    const char* carNames[] = { "RED RACER", "BLUE RACER", "GALAXY RACER", "THE TANK" };
    int totalCars = 4;
    int selectedCarIndex = 0;
    const char* mapFiles[] = { "track1.txt", "track2.txt" };
    const char* mapNames[] = { "Track 1", "Track 2" };
    int totalMaps = 2;
    int selectedMapIndex = 0;

    int numLaps = 3;
    int numEnemies = 1;
    float countdownTimer = 0.0f;

    State currentState = MENU;

    RenderTexture2D screen = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    RENDER_SETUP screenSetup = {screen, virtualScreenWidth, virtualScreenHeight, {0}, {0}};
    screenSetup = calculateScreenSetup(screenSetup);

    Camera2D camera = {0};
    camera.offset = (Vector2){ virtualScreenWidth / 2.0f, virtualScreenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.4f;

    //float scale = fminf((float)GetScreenWidth()/virtualScreenWidth, (float)GetScreenHeight()/virtualScreenHeight);
    while(!WindowShouldClose()){
        Vector2 mousePos = GetMousePosition();
        
        /*Vector2 virtualMouse = {
            (mousePos.x - (GetScreenWidth() - virtualScreenWidth*scale)*0.5f)/scale,
            (mousePos.y - (GetScreenHeight() - virtualScreenHeight*scale)*0.5f)/scale
        };*/

        UpdateGameMusic(&gameAudio, (int)currentState);

        if(currentState == PLAYING) {
            if(IsKeyPressed(KEY_SPACE)) {
                if(player.Ammo > 0) {
                    PlaySound(gameAudio.shoot);
                    player.Ammo = 0;
                    spawnProjectile(&itemManager, player.x, player.y, player.angle, true);
                }
                else if(player.hasBomb > 0) {
                    player.hasBomb = 0;
                    spawnBomb(&itemManager, player.x, player.y, true);
                }
                else if(player.hasShield > 0) {
                    player.hasShield = 0; // Gasta o item
                    player.shield = 50;   // Define 50 de HP extra
                }
            }
        }
            updateCar(&player, &gameMap);
            updateLaps(&player, &gameMap);

            for(int i=0; i<numEnemies; i++){
                updateEnemy(&enemies[i], &gameMap);
            }

            updateItems(&itemManager, &player, enemies, numEnemies, GetFrameTime(), &gameAudio, &gameMap);

            // 1. CHECAGEM DE VITÓRIA (PLAYER)
            if (player.currentLap >= numLaps) {
                playerWon = true;
                currentState = VICTORY;
                isGameRunning = false;
                saveData.highScore++;
                // REMOVIDO: SaveGame(&saveData); -> Não salva mais no disco
            }

            // 2. CHECAGEM DE DERROTA (INIMIGOS)
            for(int i=0; i<numEnemies; i++){
                if (enemies[i].vehicle.currentLap >= numLaps) {
                    playerWon = false;
                    winningEnemyIndex = i + 1;
                    currentState = VICTORY;
                    isGameRunning = false;
                    break;
                }
                if (enemies[i].vehicle.health <= 0) {
                    PlaySound(gameAudio.damage);
                    resetCar(&enemies[i].vehicle, (int)enemies[i].vehicle.startX, (int)enemies[i].vehicle.startY);
                    enemies[i].vehicle.maxLaps = numLaps;
                }
            }

            if (player.health <= 0) {
                PlaySound(gameAudio.damage);
                resetCar(&player, (int)player.startX, (int)player.startY);
                player.maxLaps = numLaps;
            }

            camera.target = (Vector2){ player.x, player.y };

        BeginTextureMode(screen);
            switch (currentState){
                case MENU:{
                    ClearBackground(BLACK);
                    DrawTextEx(gameTextures.titlefont, "Rock N' Roll", (Vector2){190,210},200,10,RAYWHITE);
                    DrawTextEx(gameTextures.titlefont, "Racing", (Vector2){505,350},200,10,RAYWHITE);
                    DrawTextEx(gameTextures.titlefont, "New Game", (Vector2){798,600},50,10,RAYWHITE);
                    DrawTextEx(gameTextures.titlefont, "Continue", (Vector2){798,670},50,10,RAYWHITE);
                    DrawTextEx(gameTextures.titlefont, "Exit", (Vector2){798,740},50,10,RAYWHITE);
                    if(CheckCollisionPointRec(mousePos,menuButtons[0])){
                        DrawTextEx(gameTextures.titlefont, "New Game", (Vector2){798,600},50,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            currentState = GAME_SETUP;
                            isGameRunning = false;
                        }
                    }
                    /*Color continueColor = isGameRunning ? RAYWHITE : GRAY;
                    DrawTextEx(gameTextures.titlefont, "Continue", (Vector2){798,670},50,10, continueColor);
                    if(isGameRunning && CheckCollisionPointRec(virtualMouse, menuButtons[1])){
                        DrawTextEx(gameTextures.titlefont, "Continue", (Vector2){798,670},50,10,YELLOW);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) currentState = PAUSE;
                    }*/
                    else if(CheckCollisionPointRec(mousePos,menuButtons[1])){
                        DrawTextEx(gameTextures.titlefont, "Continue", (Vector2){798,670},50,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                            currentState = PLAYING;
                    }
                    else if(CheckCollisionPointRec(mousePos,menuButtons[2])){
                        DrawTextEx(gameTextures.titlefont, "Exit", (Vector2){798,740},50,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                            goto endgame;
                    }
                    // Mostra as vitórias ATUAIS da sessão
                    //DrawTextEx(gameTextures.titlefont, TextFormat("Wins: %d", saveData.highScore), (Vector2){20, 20}, 30, 2, GOLD);
                    break;
                }
                case GAME_SETUP:{
                    ClearBackground(BLACK);
                    DrawTextEx(gameTextures.titlefont, "Choose your setup", (Vector2){460, 100}, 80, 5, WHITE);
                    /*Texture2D tex = gameTextures.carTextures[selectedCarIndex];
                    DrawTexturePro(tex,(Rectangle){0,0,tex.width,tex.height},(Rectangle){860, 250, 200, 100},(Vector2){0,0}, 0.0f, WHITE);

                    DrawText("<", btnPrevCar.x, btnPrevCar.y, 50, WHITE);
                    if(CheckCollisionPointRec(virtualMouse, btnPrevCar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedCarIndex--; if(selectedCarIndex < 0) selectedCarIndex = totalCars - 1;
                    }
                    DrawText(">", btnNextCar.x, btnNextCar.y, 50, WHITE);
                    if(CheckCollisionPointRec(virtualMouse, btnNextCar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedCarIndex = (selectedCarIndex + 1) % totalCars;
                    }
                    DrawTextEx(gameTextures.titlefont, carNames[selectedCarIndex], (Vector2){850, 360}, 30, 2, GRAY);

                    DrawText("<", btnPrevMap.x, btnPrevMap.y, 50, WHITE);
                    if(CheckCollisionPointRec(virtualMouse, btnPrevMap) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedMapIndex--; if(selectedMapIndex < 0) selectedMapIndex = totalMaps - 1;
                    }
                    DrawText(">", btnNextMap.x, btnNextMap.y, 50, WHITE);
                    if(CheckCollisionPointRec(virtualMouse, btnNextMap) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedMapIndex = (selectedMapIndex + 1) % totalMaps;
                    }
                    DrawTextEx(gameTextures.titlefont, TextFormat("Map: %s", mapNames[selectedMapIndex]), (Vector2){750, 510}, 40, 2, YELLOW);

                    DrawText("<", btnPrevLap.x, btnPrevLap.y, 50, WHITE);
                    if(CheckCollisionPointRec(virtualMouse, btnPrevLap) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(numLaps > 1) numLaps--;
                    }
                    DrawText(">", btnNextLap.x, btnNextLap.y, 50, WHITE);
                    if(CheckCollisionPointRec(virtualMouse, btnNextLap) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(numLaps < 5) numLaps++;
                    }
                    DrawTextEx(gameTextures.titlefont, TextFormat("Laps: %02d", numLaps), (Vector2){750, 610}, 40, 2, WHITE);

                    DrawText("<", btnPrevEnemy.x, btnPrevEnemy.y, 50, WHITE);
                    if(CheckCollisionPointRec(virtualMouse, btnPrevEnemy) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(numEnemies > 1) numEnemies--;
                    }
                    DrawText(">", btnNextEnemy.x, btnNextEnemy.y, 50, WHITE);
                    if(CheckCollisionPointRec(virtualMouse, btnNextEnemy) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(numEnemies < MAX_ENEMIES) numEnemies++;
                    }
                    DrawTextEx(gameTextures.titlefont, TextFormat("Enemies: %d", numEnemies), (Vector2){750, 710}, 40, 2, WHITE);

                    DrawRectangleLinesEx(setupBtnStart, 5, GREEN);
                    DrawTextEx(gameTextures.titlefont, "START RACE", (Vector2){setupBtnStart.x+20, setupBtnStart.y+20}, 50, 5, GREEN);*/
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
                        gameMap = loadMap(mapFiles[selectedMapIndex]);
                        getInitialPosition(&gameMap, &initialPosX, &initialPosY);
                        resetCar(&player, initialPosX, initialPosY);
                        player.maxLaps = numLaps;
                        player.carTexture = gameTextures.carTextures[selectedCarIndex];

                        for(int i=0; i<numEnemies; i++){
                            initEnemy(&enemies[i], &gameMap, gameTextures.carTextures[(selectedCarIndex + 1 + i) % 4], i);
                        }

                        initItems(&itemManager, &gameMap);
                        camera.target = (Vector2){ player.x, player.y };
                        countdownTimer = 4.0f;
                        PlaySound(gameAudio.countdown);
                        currentState = COUNTDOWN;
                        isGameRunning = true;
                    }
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
                    ClearBackground(GetColor(0x2d2d2dFF));
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
                    ClearBackground(GetColor(0x2d2d2dFF));
                    BeginMode2D(camera);
                        drawMap(&gameMap, &gameTextures);
                        drawItems(&itemManager, &gameTextures);
                        for(int i=0; i<numEnemies; i++) drawEnemy(&enemies[i]);
                        drawCar(&player);
                    EndMode2D();
                    drawHUD(&player, enemies, numEnemies, &gameTextures, virtualScreenWidth, virtualScreenHeight);
                    if(IsKeyPressed(KEY_ESCAPE)) currentState = PAUSE;
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
                    ClearBackground(BLACK);
                    if (playerWon) {
                        DrawTextEx(gameTextures.titlefont, "VICTORY!", (Vector2){650, 300}, 150, 5, GOLD);
                    } else {
                        DrawTextEx(gameTextures.titlefont, "DEFEAT...", (Vector2){680, 300}, 150, 5, RED);
                        if(winningEnemyIndex > 0) {
                            DrawTextEx(gameTextures.titlefont, TextFormat("Enemy %d Won", winningEnemyIndex), (Vector2){750, 450}, 50, 2, GRAY);
                        } else {
                            DrawTextEx(gameTextures.titlefont, "Enemy Won", (Vector2){800, 450}, 50, 2, GRAY);
                        }
                    }
                    DrawTextEx(gameTextures.titlefont, "Press ENTER to Menu", (Vector2){600, 600}, 40, 5, WHITE);
                    if(IsKeyPressed(KEY_ENTER)) currentState = MENU;
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
