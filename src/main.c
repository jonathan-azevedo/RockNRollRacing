#include <raylib.h>
#include "car.h"
#include "ui.h"
#include "map.h"
#include "textures.h"
#include "audio.h"
#include "itens.h"
#include "enemy.h"
#include <stdio.h>
#include <string.h>
#include "save.h"


//enum para facilitar saber o estado do jogo
typedef enum {MENU, GAME_SETUP, PLAYING, VICTORY, PAUSE, COUNTDOWN} State;

int main(){
    //inicializacao do jogo,variaveis e botoes
    ChangeDirectory(GetApplicationDirectory());
    const int virtualScreenWidth = 1920;
    const int virtualScreenHeight = 1080;

    InitWindow(0, 0, "Rock N' Roll Racing");
    SetWindowState(FLAG_FULLSCREEN_MODE);
    SetExitKey(0);
    InitAudioDevice();

    GAME_TEXTURES gameTextures = loadTextures();
    GameAudio gameAudio = LoadGameAudio();

    MAP gameMap = {0};
    int initialPosX = 0, initialPosY = 0;

    CAR player = {0};
    ENEMY enemies[MAX_ENEMIES] = {0};
    ITEM_MANAGER itemManager = {0};

    bool playerWon = false;
    bool isGameRunning = false;
    int winningEnemyIndex = -1;

    Rectangle menuButtons[3] = {{798,600,385,60},{798,670,385,60},{798,740,160,60}};
    Rectangle pauseMenuButtons[3] = {{785,510,345,60},{715,600,480,60},{715,690,480,50}};

    // retângulos da tela de setup
    Rectangle setupBtnBack = { 50, 50, 150, 50 };
    Rectangle setupBtnStart = { 760, 900, 400, 80 };
    
    Rectangle btnPrevCar = { 600, 300, 50, 50 };
    Rectangle btnNextCar = { 1270, 300, 50, 50 };
    Rectangle btnPrevMap = { 600, 500, 50, 50 };
    Rectangle btnNextMap = { 1270, 500, 50, 50 };
    Rectangle btnPrevLap = { 600, 600, 50, 50 };
    Rectangle btnNextLap = { 1270, 600, 50, 50 };
    Rectangle btnPrevEnemy = { 600, 700, 50, 50 };
    Rectangle btnNextEnemy = { 1270, 700, 50, 50 };

    int totalCars = 4;
    int selectedCarIndex = 0;
    const char* mapFiles[] = { "track1.txt", "track2.txt","track3.txt"};
    const char* mapNames[] = { "Track 1", "Track 2", "Track 3" };
    int totalMaps = 3;
    int selectedMapIndex = 0;

    int numLaps = 3;
    int numEnemies = 1;
    float countdownTimer = 0.0f;

    State currentState = MENU;
    //tempo para mostrar msg quando carregar o save
    float feedbackTimer = 0.0f;
    char feedbackMessage[64] = {0};
    Color feedbackColor = WHITE;

    RenderTexture2D screen = LoadRenderTexture(virtualScreenWidth, virtualScreenHeight);
    RENDER_SETUP screenSetup = {screen, virtualScreenWidth, virtualScreenHeight, {0}, {0}};
    screenSetup = calculateScreenSetup(screenSetup);

    //camera
    Camera2D camera = {0};
    camera.offset = (Vector2){ virtualScreenWidth / 2.0f, virtualScreenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.4f;

    while(!WindowShouldClose()){
        //loop principal do jogo de acordo com o state

        // traduz o clique da Tela Real para a Tela Virtual
        Vector2 realMousePos = GetMousePosition();
        Vector2 virtualMousePos;
        virtualMousePos.x = (realMousePos.x - screenSetup.destRec.x) * (screenSetup.virtualScreenWidth / screenSetup.destRec.width);
        virtualMousePos.y = (realMousePos.y - screenSetup.destRec.y) * (screenSetup.virtualScreenHeight / screenSetup.destRec.height);
        
        UpdateGameMusic(&gameAudio, (int)currentState);
        if (feedbackTimer > 0) {
            feedbackTimer -= GetFrameTime();
        }
        
        if(currentState == PLAYING){
            //input para usar itens
            if(IsKeyPressed(KEY_SPACE)){
                if(player.Ammo > 0){
                    PlaySound(gameAudio.shoot);
                    player.Ammo = 0;
                    spawnProjectile(&itemManager, player.x, player.y, player.angle, true);
                }
                else if(player.hasBomb > 0){
                    player.hasBomb = 0;
                    spawnBomb(&itemManager, player.x, player.y, -1); // Mudou de 'true' para '-1'
                }
                else if (player.hasShield > 0) {
                    player.hasShield = 0;
                    player.shieldTimer = 20.0f;
                }
                else if (player.hasNitro > 0) {
                    player.hasNitro = 0;
                    player.NitroTimer = 5.0f; 
                }
            }
            updateCar(&player, &gameMap);
            updateLaps(&player, &gameMap);

            if (player.shieldTimer > 0) {
                player.shieldTimer -= GetFrameTime();
                if (player.shieldTimer < 0) player.shieldTimer = 0.0f;
            }

            for(int i=0; i<numEnemies; i++){
                if(enemies[i].vehicle.shieldTimer > 0) {
                    enemies[i].vehicle.shieldTimer -= GetFrameTime();
                    if(enemies[i].vehicle.shieldTimer < 0) enemies[i].vehicle.shieldTimer = 0.0f;
                }
            }
            //atualizacao dos inimigos
            for(int i=0; i<numEnemies; i++){
                if(enemies[i].vehicle.health > 0){
                    updateEnemy(&enemies[i], &gameMap);
                    

                    resolveCarToCarCollision(&player, &enemies[i].vehicle, &gameMap);
                    

                    for(int j=i+1; j<numEnemies; j++){
                        if(enemies[j].vehicle.health > 0) {
                            resolveCarToCarCollision(&enemies[i].vehicle, &enemies[j].vehicle, &gameMap);
                        }
                    }
                }
            }

            updateItems(&itemManager, &player, enemies, numEnemies, GetFrameTime(), &gameAudio, &gameMap);

            // vitória
            if(player.currentLap >= numLaps){
                playerWon = true;
                currentState = VICTORY;
                isGameRunning = false;
            }
            if(player.health <= 0){
                PlaySound(gameAudio.damage);
                playerWon = false;
                currentState = VICTORY; 
                isGameRunning = false;
            }
            // verificação vitoria ou morte dos inimigos
            int activeEnemies = 0;
            for(int i=0; i<numEnemies; i++){
                if (enemies[i].vehicle.health <= 0) {
                    if (enemies[i].vehicle.x > -5000) { 
                        PlaySound(gameAudio.damage);
                        enemies[i].vehicle.x = -10000; 
                        enemies[i].vehicle.y = -10000;
                    }
                }else{
                    if(enemies[i].vehicle.currentLap >= numLaps){
                        playerWon = false;
                        winningEnemyIndex = i + 1;
                        currentState = VICTORY;
                        isGameRunning = false;
                        break;
                    }
                    activeEnemies++;
                }
            }
            // vitória por w.o,todos inimigos mortos
            if(isGameRunning && activeEnemies == 0){
                playerWon = true;
                currentState = VICTORY;
                isGameRunning = false;
            }

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
                    
                    if(CheckCollisionPointRec(virtualMousePos,menuButtons[0])){
                        DrawTextEx(gameTextures.titlefont, "New Game", (Vector2){798,600},50,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            currentState = GAME_SETUP;
                            isGameRunning = false;
                        }
                    }
                    else if(CheckCollisionPointRec(virtualMousePos, menuButtons[1])){
                        DrawTextEx(gameTextures.titlefont, "Continue", (Vector2){798,670},50,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            if(LoadGame(&player, enemies, &itemManager, &selectedMapIndex, &numLaps, &numEnemies, &selectedCarIndex)) {
                                gameMap = loadMap(mapFiles[selectedMapIndex]);
                                player.carTexture = gameTextures.carTextures[selectedCarIndex];
                                
                                for(int i = 0; i < numEnemies; i++){
                                    enemies[i].vehicle.carTexture = gameTextures.carTextures[(selectedCarIndex + 1 + i) % 4];
                                }
                                camera.target = (Vector2){ player.x, player.y };

                                // reinicia via countdown 
                                countdownTimer = 3.0f; 
                                PlaySound(gameAudio.countdown);
                                currentState = COUNTDOWN;
                                isGameRunning = true;
                                
                            } else {
                                snprintf(feedbackMessage, 64, "NO SAVE FOUND!");
                                feedbackColor = RED;
                                feedbackTimer = 2.0f;
                            }
                        }
                    }
                    if (feedbackTimer > 0 && currentState == MENU) {
                        Vector2 textSize = MeasureTextEx(gameTextures.titlefont, feedbackMessage, 40, 5);
                        DrawTextEx(gameTextures.titlefont, feedbackMessage, 
                            (Vector2){(virtualScreenWidth - textSize.x)/2, 530}, 40, 5, feedbackColor);
                    }
                    else if(CheckCollisionPointRec(virtualMousePos,menuButtons[2])){
                        DrawTextEx(gameTextures.titlefont, "Exit", (Vector2){798,740},50,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                            goto endgame;
                    }
                    break;
                }
                case GAME_SETUP:{
                    //selecao das condicoes
                    ClearBackground(BLACK);
                    DrawTextEx(gameTextures.titlefont, "Choose your setup", (Vector2){460, 100}, 80, 5, WHITE);
                    
                    Texture2D currentCarTex = gameTextures.carTextures[selectedCarIndex];
                    DrawTexturePro(currentCarTex,(Rectangle){0, 0, currentCarTex.width, currentCarTex.height},(Rectangle){860, 250, 200, 100},(Vector2){0,0}, 0.0f, WHITE);                                      
                    
                    // selecao de carro
                    DrawText("<", btnPrevCar.x, btnPrevCar.y, 50, CheckCollisionPointRec(virtualMousePos, btnPrevCar) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(virtualMousePos, btnPrevCar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedCarIndex--; if(selectedCarIndex < 0) selectedCarIndex = totalCars - 1;
                    }
                    DrawText(">", btnNextCar.x, btnNextCar.y, 50, CheckCollisionPointRec(virtualMousePos, btnNextCar) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(virtualMousePos, btnNextCar) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedCarIndex = (selectedCarIndex + 1) % totalCars;
                    }

                    // selecao de mapa
                    DrawTextEx(gameTextures.titlefont, TextFormat("Map: %s", mapNames[selectedMapIndex]), (Vector2){790, 500}, 40, 2, WHITE);
                    DrawText("<", btnPrevMap.x, btnPrevMap.y, 50, CheckCollisionPointRec(virtualMousePos, btnPrevMap) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(virtualMousePos, btnPrevMap) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedMapIndex--; if(selectedMapIndex < 0) selectedMapIndex = totalMaps - 1;
                    }
                    DrawText(">", btnNextMap.x, btnNextMap.y, 50, CheckCollisionPointRec(virtualMousePos, btnNextMap) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(virtualMousePos, btnNextMap) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        selectedMapIndex = (selectedMapIndex + 1) % totalMaps;
                    }

                    // selecao de quantidade de voltas
                    DrawTextEx(gameTextures.titlefont, TextFormat("Laps: %02d", numLaps), (Vector2){850, 600}, 40, 2, WHITE);
                    DrawText("<", btnPrevLap.x, btnPrevLap.y, 50, CheckCollisionPointRec(virtualMousePos, btnPrevLap) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(virtualMousePos, btnPrevLap) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(numLaps > 1) numLaps--;
                    }
                    DrawText(">", btnNextLap.x, btnNextLap.y, 50, CheckCollisionPointRec(virtualMousePos, btnNextLap) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(virtualMousePos, btnNextLap) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(numLaps < 50) numLaps++;
                    }

                    // selecao de quantidade de inimigos
                    DrawTextEx(gameTextures.titlefont, TextFormat("Enemies: %d", numEnemies), (Vector2){830, 700}, 40, 2, WHITE);
                    DrawText("<", btnPrevEnemy.x, btnPrevEnemy.y, 50, CheckCollisionPointRec(virtualMousePos, btnPrevEnemy) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(virtualMousePos, btnPrevEnemy) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(numEnemies > 0) numEnemies--;
                    }
                    DrawText(">", btnNextEnemy.x, btnNextEnemy.y, 50, CheckCollisionPointRec(virtualMousePos, btnNextEnemy) ? YELLOW : WHITE);
                    if(CheckCollisionPointRec(virtualMousePos, btnNextEnemy) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(numEnemies < 2) numEnemies++;
                    }

                    // botao de start
                    Color startColor = CheckCollisionPointRec(virtualMousePos, setupBtnStart) ? YELLOW : GREEN;
                    DrawRectangleLinesEx(setupBtnStart, 5, startColor);
                    DrawTextEx(gameTextures.titlefont, "START RACE", (Vector2){setupBtnStart.x+ 10, setupBtnStart.y + 15}, 48, 5, startColor);

                    if(CheckCollisionPointRec(virtualMousePos, setupBtnStart) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
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

                    DrawTextEx(gameTextures.titlefont, "BACK", (Vector2){setupBtnBack.x, setupBtnBack.y}, 30, 2, CheckCollisionPointRec(virtualMousePos, setupBtnBack)?RED:WHITE);
                    if(CheckCollisionPointRec(virtualMousePos, setupBtnBack) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        currentState = MENU;
                    }
                    break;
                }
                case COUNTDOWN: {
                    countdownTimer -= GetFrameTime();
                    if (countdownTimer <= 0.0f) {
                        currentState = PLAYING;
                    }
                    ClearBackground(GetColor(0x2d2d2dFF));
                    BeginMode2D(camera);
                        drawMap(&gameMap, &gameTextures);
                        for(int i=0; i<numEnemies; i++) {
                            drawEnemy(&enemies[i]);
                        }
                        drawCar(&player);
                    EndMode2D();
                    
                    const char* text;
                    Color color = RED;
                    int fontSize = 300;

                    if (countdownTimer > 1.0f) {
                        text = TextFormat("%d", (int)countdownTimer);
                        color = (countdownTimer > 2.0f) ? RED : ORANGE;
                    } else {
                        text = "GO!";
                        color = GREEN;
                    }
                    Vector2 textSize = MeasureTextEx(gameTextures.titlefont, text, fontSize, 10);
                    DrawTextEx(gameTextures.titlefont, text,(Vector2){(virtualScreenWidth - textSize.x)/2, (virtualScreenHeight - textSize.y)/2},fontSize, 10, color);
                    break;
                }
                case PLAYING:{
                    ClearBackground(GetColor(0x2d2d2dFF));
                    BeginMode2D(camera);
                        drawMap(&gameMap, &gameTextures);
                        drawItems(&itemManager, &gameTextures);
                        for(int i=0; i<numEnemies; i++) {
                            if(enemies[i].vehicle.health > 0)
                                drawEnemy(&enemies[i]);
                        }
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
                    
                    // Botão Resume
                    if(CheckCollisionPointRec(virtualMousePos,pauseMenuButtons[0])){
                        DrawTextEx(gameTextures.titlefont, "Resume", (Vector2){785,510},60,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                            currentState = PLAYING;
                    }
                    // Botão Save Game
                    else if(CheckCollisionPointRec(virtualMousePos,pauseMenuButtons[1])){
                        DrawTextEx(gameTextures.titlefont, "Save game", (Vector2){715,600},60,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            // Tenta salvar e pega o resultado
                            bool saved = SaveGame(&player, enemies, &itemManager, selectedMapIndex, numLaps, numEnemies, selectedCarIndex);
                            
                            if (saved) {
                                snprintf(feedbackMessage, 64, "GAME SAVED!");
                                feedbackColor = GREEN;
                            } else {
                                snprintf(feedbackMessage, 64, "SAVE ERROR!");
                                feedbackColor = RED;
                            }
                            feedbackTimer = 2.0f;
                        }
                    }
                    // botão main menu
                    else if(CheckCollisionPointRec(virtualMousePos,pauseMenuButtons[2])){
                        DrawTextEx(gameTextures.titlefont, "Main menu", (Vector2){715,690},60,10,LIGHTGRAY);
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                            currentState = MENU;
                    }

                    if (feedbackTimer > 0) {
                        DrawTextEx(gameTextures.titlefont, feedbackMessage, (Vector2){1220, 610}, 40, 5, feedbackColor);
                    }
                    break;
                }
                case VICTORY:{
                    ClearBackground(BLACK);
                    if (playerWon) {
                        DrawTextEx(gameTextures.titlefont, "VICTORY!", (Vector2){580, 300}, 150, 5, GOLD);
                    } else {
                        DrawTextEx(gameTextures.titlefont, "DEFEAT...", (Vector2){620, 300}, 150, 5, RED);
                        if(winningEnemyIndex > 0) {
                            DrawTextEx(gameTextures.titlefont, TextFormat("Enemy %d Won", winningEnemyIndex), (Vector2){650, 450}, 50, 2, GRAY);
                        } else {
                            DrawTextEx(gameTextures.titlefont, "Enemy Won", (Vector2){600, 450}, 50, 2, GRAY);
                        }
                    }
                    DrawTextEx(gameTextures.titlefont, "Press ENTER to Menu", (Vector2){640, 600}, 40, 5, WHITE);
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
}