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