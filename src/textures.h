#pragma once
#include <raylib.h>


typedef struct{
    Texture2D carTextures[4];
    Texture2D asphaltTexture;
    Texture2D lineTexture;
    Texture2D ammoTexture;
    Texture2D lifeTexture;
    Texture2D shieldTexture;

    Font titlefont;

}GAME_TEXTURES;

GAME_TEXTURES loadTextures();
void unloadTextures(GAME_TEXTURES *textures);