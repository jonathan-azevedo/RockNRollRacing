#include "textures.h"

GAME_TEXTURES loadTextures() {
    //carrega todos os arquivos de imagem e e os organiza dentro de uma estrutura para uso no jogo
    GAME_TEXTURES textures = {0};

    // veiculos
    textures.carTextures[0] = LoadTexture("Assets/Textures/redCar.png");
    textures.carTextures[1] = LoadTexture("Assets/Textures/blueCar.png");
    textures.carTextures[2] = LoadTexture("Assets/Textures/spaceship.png");
    textures.carTextures[3] = LoadTexture("Assets/Textures/tank.png");

    // pista
    textures.asphaltTexture = LoadTexture("Assets/Textures/asphalt.png");
    textures.lineTexture    = LoadTexture("Assets/Textures/line.png");

    // gameplay
    textures.projectile = LoadTexture("Assets/Textures/bullet.png");
    textures.explosion  = LoadTexture("Assets/Textures/explosion.png");
    textures.luckBox    = LoadTexture("Assets/Textures/luck_box.png");
    textures.bomb = LoadTexture("Assets/Textures/bomb.png");

    // hud
    textures.ammoTexture   = LoadTexture("Assets/Textures/icon_ammo.png");
    textures.lifeTexture   = LoadTexture("Assets/Textures/icon_life.png");
    textures.shieldTexture = LoadTexture("Assets/Textures/icon_shield.png");
    textures.speedTexture  = LoadTexture("Assets/Textures/speed.png");

    // fonte
    textures.titlefont = LoadFontEx("Assets/Fonts/RaceSport.ttf", 250, 0, 0);

    return textures;
}

void unloadTextures(GAME_TEXTURES *textures) {
    //libera a memoria ocupada por cada textura e fonte carregada
    UnloadTexture(textures->carTextures[0]);
    UnloadTexture(textures->carTextures[1]);
    UnloadTexture(textures->carTextures[2]);
    UnloadTexture(textures->carTextures[3]);

    UnloadTexture(textures->asphaltTexture);
    UnloadTexture(textures->lineTexture);

    UnloadTexture(textures->projectile);
    UnloadTexture(textures->explosion);
    UnloadTexture(textures->luckBox);
    UnloadTexture(textures->bomb); 

    UnloadTexture(textures->ammoTexture);
    UnloadTexture(textures->lifeTexture);
    UnloadTexture(textures->shieldTexture);
    UnloadTexture(textures->speedTexture);

    UnloadFont(textures->titlefont);
}
