#include "ui.h"
#include <raylib.h>
#include <stdio.h>

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

void drawHUD(CAR *player, ENEMY enemies[], int enemyCount, GAME_TEXTURES *textures, int screenW, int screenH){
    float iconScale = 0.2f;
    int baseY = screenH - 50;
    int barHeight = 20;
    float thick = 2.5f;

    // --- PLAYER STATS ---
    DrawTextureEx(textures->lifeTexture, (Vector2){20, baseY - 15}, 0.0f, iconScale, WHITE);
    DrawRectangle(65, baseY, 200, barHeight, Fade(MAROON, 0.8f));
    DrawRectangle(65, baseY, player->health * 2, barHeight, GREEN);
    DrawRectangleLinesEx((Rectangle){65, baseY, 200, barHeight}, thick, WHITE);
    DrawText(TextFormat("%d", player->health), 75, baseY + 2, 20, BLACK);
    DrawText(TextFormat("%d", player->health), 74, baseY + 1, 20, WHITE);

    if (player->shieldTimer > 0) {
        DrawTextureEx(textures->shieldTexture, (Vector2){280, baseY - 15}, 0.0f, iconScale, WHITE);
        
        // Calcula largura baseada em 20 segundos
        float maxTime = 20.0f;
        float currentWidth = (player->shieldTimer / maxTime) * 150.0f;
        
        DrawRectangle(325, baseY, (int)currentWidth, barHeight, Fade(SKYBLUE, 0.9f));
        DrawRectangleLinesEx((Rectangle){325, baseY, 150, barHeight}, thick, WHITE);
        
        // Mostra o tempo formatado (Ex: 15.4s)
        const char* timeText = TextFormat("%.1fs", player->shieldTimer);
        DrawText(timeText, 335, baseY + 2, 20, BLACK);
        DrawText(timeText, 334, baseY + 1, 20, WHITE);
    }

    // Inventory
    int invnX = 930;
    DrawRectangle(invnX, baseY - 10, 60, 60, Fade(BLACK, 0.5f));
    DrawRectangleLinesEx((Rectangle){invnX, baseY - 10, 60, 60}, thick, WHITE);
    
    if (player->Ammo > 0) {
        DrawTexturePro(textures->ammoTexture, (Rectangle){0,0, textures->ammoTexture.width, textures->ammoTexture.height}, (Rectangle){invnX + 30, baseY + 20, 40, 40}, (Vector2){20, 20}, 0.0f, WHITE);
        DrawText("READY", invnX + 15, baseY + 35, 10, GREEN);
    } else if (player->hasBomb > 0) {
        DrawTexturePro(textures->bomb, (Rectangle){0,0, textures->bomb.width, textures->bomb.height}, (Rectangle){invnX + 30, baseY + 20, 40, 40}, (Vector2){20, 20}, 0.0f, WHITE);
        DrawText("DROP", invnX + 18, baseY + 35, 10, RED);
    }
    else if (player->hasShield > 0) {
        DrawTexturePro(textures->shieldTexture, (Rectangle){0,0, (float)textures->shieldTexture.width, (float)textures->shieldTexture.height}, (Rectangle){invnX + 30, baseY + 20, 40, 40}, (Vector2){20, 20}, 0.0f, WHITE);
        DrawText("SHIELD", invnX + 12, baseY + 35, 10, SKYBLUE);
    }
    else if (player->hasNitro > 0) {
        DrawTexturePro(textures->speedTexture, 
            (Rectangle){0,0, textures->speedTexture.width, textures->speedTexture.height}, 
            (Rectangle){invnX + 30, baseY + 20, 40, 40}, 
            (Vector2){20, 20}, 0.0f, WHITE);
        DrawText("NITRO", invnX + 18, baseY + 35, 10, YELLOW);
    }
    else {
        DrawText("EMPTY", invnX + 10, baseY + 15, 10, GRAY);
    }
    DrawText("Inventory", invnX + 5, baseY - 25, 10, WHITE);

    // INFO
    DrawText(TextFormat("LAPS: %d/%d", player->currentLap, player->maxLaps), 1420, baseY, 30, BLACK);
    DrawText(TextFormat("LAPS: %d/%d", player->currentLap, player->maxLaps), 1418, baseY - 2, 30, YELLOW);

// --- ENEMIES INFO (CANTO INFERIOR DIREITO) ---
    int enemyStartX = screenW - 260; 
    int lineHeight = 30;
    
    // Calcula a posição inicial (topo do bloco) para que o bloco termine alinhado com o 'baseY'
    int totalBlockHeight = enemyCount * lineHeight;
    int startY = baseY - totalBlockHeight + 20; // +20 para ajustar com a linha de texto do player

    for (int i = 0; i < enemyCount; i++) {
        int yPos = startY + (i * lineHeight);
        Color textColor = (enemies[i].vehicle.health > 0) ? RED : GRAY;
        const char* enemyText = TextFormat("ENEMY %d: %d (Laps: %d)", i+1, enemies[i].vehicle.health, enemies[i].vehicle.currentLap);

        DrawText(enemyText, enemyStartX + 2, yPos + 2, 22, BLACK); // Sombra
        DrawText(enemyText, enemyStartX, yPos, 22, textColor);     // Texto
    }
}