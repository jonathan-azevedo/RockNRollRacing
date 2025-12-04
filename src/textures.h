#pragma once
#include <raylib.h>


typedef struct{
    Texture2D carTexture;
    Texture2D asphaltTexture;
    Texture2D stripTexture;
    Font titlefont;

}GAME_TEXTURES;

GAME_TEXTURES loadTextures();
void unloadTextures(GAME_TEXTURES *textures);