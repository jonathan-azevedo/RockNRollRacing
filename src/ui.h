#pragma once
#include <raylib.h>
#include <math.h>
#include "car.h"
#include "textures.h"



typedef struct{
    RenderTexture2D screen;
    int virtualScreenWidth;
    int virtualScreenHeight;
    Rectangle sourceRec;
    Rectangle destRec;
}RENDER_SETUP;


RENDER_SETUP calculateScreenSetup(RENDER_SETUP renderSetup);
void drawHUD(CAR *player, CAR *enemyVehicle, GAME_TEXTURES *textures, int screenW, int screenH);
