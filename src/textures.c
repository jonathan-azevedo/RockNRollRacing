#include "textures.h"





GAME_TEXTURES loadTextures() {
    GAME_TEXTURES textures = {0};
    textures.carTextures[0] = LoadTexture("Assets/Textures/redCar.png");
    textures.carTextures[1] = LoadTexture("Assets/Textures/blueCar.png"); 
    textures.carTextures[2] = LoadTexture("Assets/Textures/spaceship.png");
    textures.carTextures[3] = LoadTexture("Assets/Textures/tank.png");
    textures.asphaltTexture = LoadTexture("Assets/Textures/asphalt.png");
    textures.stripTexture = LoadTexture("Assets/Textures/strip.png");
    textures.titlefont = LoadFontEx("Assets/Fonts/RaceSport.ttf",1080,0,0);
    return textures;

}

void unloadTextures(GAME_TEXTURES *textures) {
    UnloadTexture(textures->carTextures[0]);
    UnloadTexture(textures->carTextures[1]);
    UnloadTexture(textures->carTextures[2]);
    UnloadTexture(textures->carTextures[3]);
    UnloadTexture(textures->asphaltTexture);
    UnloadTexture(textures->stripTexture);
    UnloadFont(textures->titlefont);
}