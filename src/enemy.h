#pragma once
#include "car.h"
#include "map.h"
#define MAX_ENEMIES 3

typedef struct {
    CAR vehicle;
    float turnTimer;
    float turnDuration;
    int turnDirection;
    int id; //  um ID para diferenciar os inimigos 
} ENEMY;

//  
void initEnemy(ENEMY *enemy, MAP *map, Texture2D texture, int offsetIndex); //inicializa o inimigo 
void updateEnemy(ENEMY *enemy, MAP *map); //atualiza o inimigo
void drawEnemy(ENEMY *enemy); //desenha o inimigo
float getWallDistance(MAP *map, float startX, float startY, float angleDeg); //sensor de distancia
