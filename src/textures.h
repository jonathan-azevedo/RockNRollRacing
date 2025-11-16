#pragma once
#include <raylib.h>


typedef struct{
    Texture2D carTexture;
    Texture2D asphaltTexture;

}GAME_TEXTURES;

GAME_TEXTURES loadTextures();
void unloadTextures(GAME_TEXTURES *textures);