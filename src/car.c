#include "car.h"
#include <math.h>

void drawCar(CAR *car) {
    Rectangle sourceRec = { 0.0f, 0.0f, (float)car->carTexture.width, (float)car->carTexture.height };
    Rectangle destRec = { car->x, car->y, car->carTexture.width, car->carTexture.height };
    Vector2 origin = { car->carTexture.width / 2.0f, car->carTexture.height / 2.0f };

    DrawTexturePro(car->carTexture, sourceRec, destRec, origin, car->angle, WHITE);
    
    // Debug: Visualizar a hitbox física
    // DrawRectangleLinesEx(car->collider, 1, RED);
}

int isWall(MAP *map, float x, float y) {
    int gridX = (int)(x / TILE_SIZE);
    int gridY = (int)(y / TILE_SIZE);

    if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT) return 1;

    char tile = map->tiles[gridY][gridX];
    // Tiles permitidos (Pista) - Lista original
    if (tile == ' ' || tile == 'j' || tile == 'i' || tile == 'l') return 0;

    return 1;
}

int checkCarCollision(CAR *car, MAP *map) {
    // --- RESTAURAÇÃO DA FÍSICA CLÁSSICA ---
    // Voltamos a usar as dimensões FIXAS (60x28) originais do jogo antigo.
    // Ignorar o tamanho da textura evita que texturas novas maiores causem
    // o bug de "travar" na parede. A física volta a ser sólida como antes.
    float useWidth = 60.0f;
    float useHeight = 28.0f;

    float halfW = useWidth / 2.0f;
    float halfH = useHeight / 2.0f;

    float rad = car->angle * DEG2RAD;
    float s = sinf(rad);
    float c = cosf(rad);

    float corners[4][2] = {
        { -halfW, -halfH },
        { +halfW, -halfH },
        { +halfW, +halfH },
        { -halfW, +halfH }
    };

    for (int i = 0; i < 4; i++) {
        float rotatedX = (corners[i][0] * c) - (corners[i][1] * s);
        float rotatedY = (corners[i][0] * s) + (corners[i][1] * c);

        float finalX = car->x + rotatedX;
        float finalY = car->y + rotatedY;

        if (isWall(map, finalX, finalY)) return 1;
    }
    return 0;
}

void updateCar(CAR *car, MAP *gameMap) {
    float deltaTime = GetFrameTime();
    
    int goFront = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    int goBack = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    int turnLeft = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    int turnRight = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

    if(goFront) {
        car->currentSpeed += car->acceleration * deltaTime;
        if (car->currentSpeed < 0) car->currentSpeed += car->brakeSpeed * deltaTime;
    } 
    else if(goBack) {
        car->currentSpeed -= car->acceleration * 0.85f * deltaTime; 
        if (car->currentSpeed > 0) car->currentSpeed -= car->brakeSpeed * deltaTime;
    } 
    else {
        if (car->currentSpeed > 0) {
            car->currentSpeed -= car->friction * deltaTime;
            if (car->currentSpeed < 0) car->currentSpeed = 0; 
        } else if (car->currentSpeed < 0) {
            car->currentSpeed += car->friction * deltaTime;
            if (car->currentSpeed > 0) car->currentSpeed = 0;
        }
    }

    if (car->currentSpeed > car->maxSpeed) car->currentSpeed = car->maxSpeed;
    if (car->currentSpeed < -car->maxSpeed * 0.85f) car->currentSpeed = -car->maxSpeed * 0.85f; 
    
    float oldAngle = car->angle; 

    if (fabs(car->currentSpeed) > 0.1f || goFront || goBack) {
        if(turnLeft)  car->angle -= (car->currentSpeed * 0.62f) * deltaTime;
        if(turnRight) car->angle += (car->currentSpeed * 0.62f) * deltaTime;
    }
    car->angle = fmod(car->angle + 360.0f, 360.0f);

    if (checkCarCollision(car, gameMap)) {
        car->angle = oldAngle; 
    }

    float moveX = cosf(car->angle * DEG2RAD) * car->currentSpeed * deltaTime;
    float moveY = sinf(car->angle * DEG2RAD) * car->currentSpeed * deltaTime;
    float wallFriction = 460.0f * deltaTime; 

    car->x += moveX;
    if (checkCarCollision(car, gameMap)) {
        car->x -= moveX;  
        if(car->currentSpeed > 0) {
            car->currentSpeed -= wallFriction;
            if(car->currentSpeed < 0) car->currentSpeed = 0;
        } else if(car->currentSpeed < 0) {
            car->currentSpeed += wallFriction;
            if(car->currentSpeed > 0) car->currentSpeed = 0;
        }
    }

    // EIXO Y
    car->y += moveY;
    if (checkCarCollision(car, gameMap)) {
        car->y -= moveY; 
        if(car->currentSpeed > 0) {
            car->currentSpeed -= wallFriction;
            if(car->currentSpeed < 0) car->currentSpeed = 0;
        } else if(car->currentSpeed < 0) {
            car->currentSpeed += wallFriction;
            if(car->currentSpeed > 0) car->currentSpeed = 0;
        }
    }
    car->collider = (Rectangle){ car->x - 30, car->y - 14,60,28};
}
void updateEnemyCar(CAR *car, MAP *gameMap) {
    float deltaTime = GetFrameTime();

    // Limites de velocidade para o inimigo
    if (car->currentSpeed > car->maxSpeed) car->currentSpeed = car->maxSpeed;
    if (car->currentSpeed < -car->maxSpeed * 0.85f) car->currentSpeed = -car->maxSpeed * 0.85f; 
    
    // AVISO: O arquivo enemy.c altera o ângulo ANTES de chamar esta função.
    // Isso faz 'oldAngle' já ser o ângulo "perigoso".
    // A lógica abaixo tenta validar, mas se a IA já colocou dentro da parede, não há "oldAngle" seguro aqui.
    float oldAngle = car->angle; 

    car->angle = fmod(car->angle + 360.0f, 360.0f);

    if (checkCarCollision(car, gameMap)) {
        car->angle = oldAngle; // Tenta reverter, mas pode falhar se oldAngle já for inválido
    }

    float moveX = cosf(car->angle * DEG2RAD) * car->currentSpeed * deltaTime;
    float moveY = sinf(car->angle * DEG2RAD) * car->currentSpeed * deltaTime;
    float wallFriction = 460.0f * deltaTime; 

    car->x += moveX;
    if (checkCarCollision(car, gameMap)) {
        car->x -= moveX;  
        if(car->currentSpeed > 0) {
            car->currentSpeed -= wallFriction;
            if(car->currentSpeed < 0) car->currentSpeed = 0;
        } else if(car->currentSpeed < 0) {
            car->currentSpeed += wallFriction;
            if(car->currentSpeed > 0) car->currentSpeed = 0;
        }
    }

    car->y += moveY;
    if(checkCarCollision(car, gameMap)) {
        car->y -= moveY; 
        if(car->currentSpeed > 0) {
            car->currentSpeed -= wallFriction;
            if(car->currentSpeed < 0) car->currentSpeed = 0;
        } else if(car->currentSpeed < 0) {
            car->currentSpeed += wallFriction;
            if(car->currentSpeed > 0) car->currentSpeed = 0;
        }
    }
    
    // CORREÇÃO 3: Atualiza colisor do inimigo (Faltava isso!)
    // Sem isso, tiros e caixas não funcionam no inimigo.
    car->collider = (Rectangle){ car->x - 30, car->y - 14, 60, 28 };
}
    
void updateLaps(CAR *car, MAP *map) {
    float dx = car->x - car->lastLapX;
    float dy = car->y - car->lastLapY;
    float dist = sqrtf(dx*dx + dy*dy);

    if (dist > 2000.0f) car->hasReachedMidpoint = 1;

    if (isTouchingTile(car, map, 'l')) {
        if (car->hasReachedMidpoint == 1 && car->angle > 178.0f && car->angle < 360.0f) {
            car->currentLap++;
        }
        car->lastLapX = car->x;
        car->lastLapY = car->y;
        car->hasReachedMidpoint = 0;
    }
}

int isTouchingTile(CAR *car, MAP *map, char targetTile) {
    int gridX = (int)(car->x / TILE_SIZE);
    int gridY = (int)(car->y / TILE_SIZE);
    if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT) return 0;
    return (map->tiles[gridY][gridX] == targetTile);
}

void resetCar(CAR *car, int initialPosX, int initialPosY){
    int wasEnemy = car->isEnemy;

    car->x = (float)initialPosX;
    car->y = (float)initialPosY;
    car->angle = 0.0f;
    
    // Valores do jogo original
    car->width = 60.0f;
    car->height = 28.0f;
    
    car->currentSpeed = 0.0f;
    car->maxSpeed = 420.0f;
    car->acceleration = 250.0f;
    car->brakeSpeed = 300.0f;
    car->friction = 200.0f;
    car->currentLap = 0;
    car->maxLaps = 0;
    car->hasReachedMidpoint = 0;
    car->lastLapX = (float)initialPosX;
    car->lastLapY = (float)initialPosY;
    car->health = 100;
    car->shield = 0;
    car->hasShield = 0;
    car->Ammo = 0;
    car->hasBomb = 0;
    car->isEnemy = wasEnemy;
    car->startX = (float)initialPosX;
    car->startY = (float)initialPosY;
    
    // Inicializa collider
    car->collider = (Rectangle){ car->x - 30, car->y - 14, 60, 28 };
}