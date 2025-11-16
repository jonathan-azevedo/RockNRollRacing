#include "textures.h"





GAME_TEXTURES loadTextures() {
    GAME_TEXTURES textures = {0};
    textures.carTexture = LoadTexture("Assets/Textures/car.png");
    textures.asphaltTexture = LoadTexture("Assets/Textures/asphalt.png");
    return textures;

}

void unloadTextures(GAME_TEXTURES *textures) {
    UnloadTexture(textures->carTexture);
    UnloadTexture(textures->asphaltTexture);
}