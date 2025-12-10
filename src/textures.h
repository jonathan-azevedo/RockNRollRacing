#pragma once
#include <raylib.h>

typedef struct{
    Texture2D carTextures[4];
    Texture2D asphaltTexture;
    Texture2D lineTexture;

    Texture2D ammoTexture;
    Texture2D lifeTexture;
    Texture2D shieldTexture;
    Texture2D speedTexture;

    Texture2D projectile;
    Texture2D explosion;
    Texture2D luckBox;

    // ADICIONADO:
    Texture2D bomb;

    Font titlefont;

} GAME_TEXTURES;

GAME_TEXTURES loadTextures();
void unloadTextures(GAME_TEXTURES *textures);
