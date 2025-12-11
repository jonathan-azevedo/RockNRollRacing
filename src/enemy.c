#include "enemy.h"
#include <math.h>
#include <stdlib.h>


void initEnemy(ENEMY *enemy, MAP *map, Texture2D texture, int offsetIndex) {
    //configura o inimigo, encontra o ponto de spawn no mapa , e aplica variações de velocidade e comportamento baseadas no id
    int startX = 200, startY = 200;
    int iFoundCount = 0;
    int positionFound = 0;

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if(map->tiles[y][x] == 'i'){
                if (iFoundCount == offsetIndex) {
                    startX = x * TILE_SIZE + TILE_SIZE/2;
                    startY = y * TILE_SIZE + TILE_SIZE/2;
                    positionFound = 1;
                    break;
                }
                iFoundCount++;
            }
        }
        if (positionFound) break;
    }

    if (!positionFound && offsetIndex > 0) {
        startX -= (60 * offsetIndex);
    }

    resetCar(&enemy->vehicle, startX, startY);

    enemy->vehicle.carTexture = texture;
    enemy->vehicle.maxSpeed = 380.0f;
    enemy->vehicle.acceleration = 180.0f;
    enemy->vehicle.isEnemy = 1;
    enemy->id = offsetIndex;
    enemy->turnTimer = 0.0f;
    enemy->turnDuration = 0.0f; 
    enemy->turnDirection = 0;

    if (offsetIndex % 2 != 0) enemy->vehicle.maxSpeed += 15.0f;
}

int checkEnemyWall(MAP *map, float x, float y) {
    return isWall(map, x, y);
}

float getWallDistance(MAP *map, float startX, float startY, float angleDeg) {
    //sensor de distancia
    float rad = angleDeg * DEG2RAD;
    float dirX = cosf(rad);
    float dirY = sinf(rad);
    
    float dist = 0;
    float step = 15.0f; 
    float maxDist = 450.0f; 

    while (dist < maxDist) {
        float checkX = startX + dirX * dist;
        float checkY = startY + dirY * dist;
        if (isWall(map, checkX, checkY)) return dist;
        dist += step;
    }
    return maxDist;
}

void updateEnemy(ENEMY *enemy, MAP *map) {
    float dt = GetFrameTime();
    CAR *c = &enemy->vehicle;
    //recuperacao em travamento
    if (enemy->turnDuration > 0.0f) {
        enemy->turnDuration -= dt;
        
        c->currentSpeed = -120.0f; 

        float dir = (enemy->turnDirection == 1) ? 1.0f : -1.0f;
        c->angle += (130.0f * dir) * dt;

        updateEnemyCar(c, map);
        updateLaps(c, map);
        return; 
    }


    // Sensores
    float distLeft  = getWallDistance(map, c->x, c->y, c->angle - 35.0f); 
    float distRight = getWallDistance(map, c->x, c->y, c->angle + 35.0f);
    float distFront = getWallDistance(map, c->x, c->y, c->angle);


    float laneBias = (enemy->id % 2 == 0) ? 40.0f : -40.0f;
    
    //calculo direcao e controle da velocidade
    float turnForce = ((distRight - distLeft) + laneBias) * 2.5f * dt; 
    c->angle += turnForce;

    float speedTarget = c->maxSpeed;

    // curvas fechadas reduz a velocidade
    if (fabsf(distRight - distLeft) > 150.0f) {
        speedTarget = c->maxSpeed * 0.70f;
    }

    if (distFront < 140.0f) {
        speedTarget = 50.0f; 
        // curva de panico
        float emergencyTurn = (distRight > distLeft ? 180.0f : -180.0f) * dt;
        c->angle += emergencyTurn;
    }

    if (c->currentSpeed < speedTarget) {
        c->currentSpeed += c->acceleration * dt;
    } else {
        c->currentSpeed -= c->friction * dt;
    }

    // detector de travamento
    if (c->currentSpeed > -20.0f && c->currentSpeed < 15.0f) {
        enemy->turnTimer += dt;
        
        if (enemy->turnTimer > 1.0f) {
             enemy->turnDuration = 1.2f; 
             enemy->turnTimer = 0.0f;    
             
             // Decide para onde girar para ficar livre
             enemy->turnDirection = (distRight > distLeft) ? 1 : 0;
        }
    } else {
        enemy->turnTimer = 0.0f; // Reseta se estiver andando bem
    }
    
    updateEnemyCar(c, map);
    updateLaps(c, map);
}

void drawEnemy(ENEMY *enemy) {
    //desenha o inimigo
    drawCar(&enemy->vehicle);
}