#include "textures.h"





GAME_TEXTURES loadTextures() {
    GAME_TEXTURES textures = {0};
    textures.carTexture = LoadTexture("Assets/Textures/car.png");
    textures.asphaltTexture = LoadTexture("Assets/Textures/asphalt.png");
    textures.stripTexture = LoadTexture("Assets/Textures/strip.png");
    textures.titlefont = LoadFontEx("Assets/Fonts/RaceSport.ttf",1080,0,0);
    return textures;

}

void unloadTextures(GAME_TEXTURES *textures) {
    UnloadTexture(textures->carTexture);
    UnloadTexture(textures->asphaltTexture);
    UnloadTexture(textures->stripTexture);
    UnloadFont(textures->titlefont);
}