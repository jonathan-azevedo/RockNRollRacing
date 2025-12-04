#include "car.h"

void drawCar(CAR *car) {
    Rectangle sourceRec = { 0.0f, 0.0f, (float)car->carTexture.width, (float)car->carTexture.height };
    Rectangle destRec = { car->x, car->y, car->carTexture.width, car->carTexture.height };
    Vector2 origin = { car->carTexture.width / 2.0f, car->carTexture.height / 2.0f };

    DrawTexturePro(car->carTexture, sourceRec, destRec, origin, car->angle, WHITE);
}

int isWall(MAP *map, float x, float y) {
    // 1. Converter pixel para índice da grade
    int gridX = (int)(x / TILE_SIZE);
    int gridY = (int)(y / TILE_SIZE);

    if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT) {
        return 1; 
    }
    // 3. Verificar o tile
    char tile = map->tiles[gridY][gridX];
    
    if (tile == ' ' || tile == 'j' || tile == 'i' || tile == 'l') {
        return 0; 
    }
    return 1; 
}

int checkCarCollision(CAR *car, MAP *map) {
    float halfW = car->width / 2.0f;
    float halfH = car->height / 2.0f;
    float rad = car->angle * DEG2RAD;
    float s = sinf(rad);
    float c = cosf(rad);

    float corners[4][2] = {
        { -halfW, -halfH }, // Trás-Esquerda
        { +halfW, -halfH }, // Frente-Esquerda
        { +halfW, +halfH }, // Frente-Direita
        { -halfW, +halfH }  // Trás-Direita
    };

    for (int i = 0; i < 4; i++) {
        float rotatedX = (corners[i][0] * c) - (corners[i][1] * s);
        float rotatedY = (corners[i][0] * s) + (corners[i][1] * c);

        float finalX = car->x + rotatedX;
        float finalY = car->y + rotatedY;

        if (isWall(map, finalX, finalY)) {
            return 1;
        }
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
}
void updateLaps(CAR *car, MAP *map) {
    float dx = car->x - car->lastLapX;
    float dy = car->y - car->lastLapY;
    float dist = sqrtf(dx*dx + dy*dy);

    if (dist > 2000.0f) {
        car->hasReachedMidpoint = 1; 
    }

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
