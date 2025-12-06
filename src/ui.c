#include "ui.h"
#include <raylib.h>



RENDER_SETUP calculateScreenSetup(RENDER_SETUP renderSetup){
    float realScreenWidth = (float)GetScreenWidth();
    float realScreenHeight = (float)GetScreenHeight();

    float scaleX = realScreenWidth / renderSetup.virtualScreenWidth;
    float scaleY = realScreenHeight / renderSetup.virtualScreenHeight;
    float scale = fminf(scaleX, scaleY);

    float destX = (realScreenWidth - (renderSetup.virtualScreenWidth * scale)) * 0.5f;
    float destY = (realScreenHeight - (renderSetup.virtualScreenHeight * scale)) * 0.5f;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)renderSetup.screen.texture.width, -(float)renderSetup.screen.texture.height };
    Rectangle destRec = {destX, destY, (float)renderSetup.virtualScreenWidth * scale, (float)renderSetup.virtualScreenHeight * scale };
    renderSetup.sourceRec = sourceRec;
    renderSetup.destRec = destRec;
    return renderSetup;
}

void drawHUD(CAR *player, CAR *enemyVehicle, GAME_TEXTURES *textures, int screenW, int screenH){
// Configura��es
    float iconScale = 0.2f;
    int baseY = screenH - 50;
    int barHeight = 20;
    float thick = 2.5f; //

    // --- 1. VIDA (LIFE) ---
    DrawTextureEx(textures->lifeTexture, (Vector2){20, baseY - 15}, 0.0f, iconScale, WHITE);

    // Fundo da barra
    DrawRectangle(65, baseY, 200, barHeight, Fade(MAROON, 0.8f));

    // Barra de Vida Atual (Verde)
    DrawRectangle(65, baseY, player->health * 2, barHeight, GREEN);

    // === BORDA GROSSA DA VIDA ===
    // (Rectangle){x, y, largura, altura}, espessura, cor
    DrawRectangleLinesEx((Rectangle){65, baseY, 200, barHeight}, thick, WHITE);

    // Texto
    DrawText(TextFormat("%d%%", player->health), 75, baseY + 2, 20, BLACK);
    DrawText(TextFormat("%d%%", player->health), 74, baseY + 1, 20, WHITE);

    // --- 2. ESCUDO (SHIELD) ---
    // A borda s� desenha se entrar neste IF
    if (player->shield > 0) {
        DrawTextureEx(textures->shieldTexture, (Vector2){280, baseY - 15}, 0.0f, iconScale, WHITE);

        // Barra de Escudo
        DrawRectangle(325, baseY, player->shield * 2, barHeight, Fade(SKYBLUE, 0.9f));

        // === BORDA GROSSA DO ESCUDO ===
        // Largura fixa de 100 (50 de escudo * 2)
        DrawRectangleLinesEx((Rectangle){325, baseY, 100, barHeight}, thick, WHITE);

        // Texto
        DrawText(TextFormat("+%d", player->shield), 335, baseY + 2, 20, BLACK);
        DrawText(TextFormat("+%d", player->shield), 334, baseY + 1, 20, WHITE);
    }

    // --- 3. ARMA (WEAPON) ---
    DrawRectangle(600, baseY - 10, 60, 60, Fade(BLACK, 0.5f));

    //borda
    DrawRectangleLinesEx((Rectangle){600, baseY - 10, 60, 60}, thick, WHITE);

    if (player->Ammo) {
        float ammoScale = 0.8f;
        DrawTexturePro(textures->ammoTexture,
            (Rectangle){0, 0, (float)textures->ammoTexture.width, (float)textures->ammoTexture.height},
            (Rectangle){630, baseY + 20, 40, 40},
            (Vector2){20, 20}, 0.0f, WHITE);

        DrawText("READY", 615, baseY + 35, 10, GREEN);
    } else {
        DrawText("EMPTY", 610, baseY + 15, 10, GRAY);
    }


    // T�tulo Weapon pequeno em cima
    DrawText("WEAPON", 609, baseY - 25, 10, WHITE); //DrawText(texto, posX, posY, tamanho, cor);


    // --- 4. INFORMA��ES (INFO) ---
    // Usamos sombra (desenhar preto antes) para garantir contraste com o mapa

    // Voltas
    DrawText(TextFormat("LAPS: %d/%d", player->currentLap, player->maxLaps), 800, baseY, 30, BLACK);
    DrawText(TextFormat("LAPS: %d/%d", player->currentLap, player->maxLaps), 798, baseY - 2, 30, YELLOW);

    // Velocidade
    DrawText(TextFormat("SPD: %.0f", player->currentSpeed), 1000, baseY - 10, 20, BLACK);
    DrawText(TextFormat("SPD: %.0f", player->currentSpeed), 999, baseY - 11, 20, WHITE);

    // Vida do Inimigo
    DrawText(TextFormat("ENEMY: %d%%", enemyVehicle->health), 1000, baseY + 15, 20, BLACK);
    DrawText(TextFormat("ENEMY: %d%%", enemyVehicle->health), 999, baseY + 14, 20, RED);








}