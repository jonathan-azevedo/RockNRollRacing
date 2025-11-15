#pragma once
#include <raylib.h>
#include <math.h>



typedef struct{
    RenderTexture2D screen;
    int virtualScreenWidth;
    int virtualScreenHeight;
    Rectangle sourceRec;
    Rectangle destRec;
}RENDER_SETUP;


RENDER_SETUP calculateScreenSetup(RENDER_SETUP renderSetup);
