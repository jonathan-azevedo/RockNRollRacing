#pragma once
#include <raylib.h>
#include <math.h>
#include "car.h"
#include "textures.h"
#include "enemy.h"

typedef struct{
    RenderTexture2D screen;
    int virtualScreenWidth;
    int virtualScreenHeight;
    Rectangle sourceRec;
    Rectangle destRec;
} RENDER_SETUP;

RENDER_SETUP calculateScreenSetup(RENDER_SETUP renderSetup);

// AGORA RECEBE ARRAY DE INIMIGOS
void drawHUD(CAR *player, ENEMY enemies[], int enemyCount, GAME_TEXTURES *textures, int screenW, int screenH);
