#pragma once
#include <raylib.h>
#include <math.h>
#include "map.h"

typedef struct{
    float x;
    float y;
    float angle;
    float width;
    float height;
    Texture2D carTexture;

    // F�sica
    float currentSpeed;
    float maxSpeed;
    float acceleration;
    float brakeSpeed;
    float friction;

    // Gameplay
    int currentLap;
    int maxLaps;
    int hasReachedMidpoint;
    float lastLapX;
    float lastLapY;

    // Status
    int health;
    int shield;
    float shieldTimer;
    int hasShield;
    int Ammo;
    int hasBomb;

    // Flags
    int isEnemy;
    Rectangle collider;

    // Respawn
    float startX;
    float startY;

} CAR;

void drawCar(CAR *car);
void updateCar(CAR *car, MAP *gameMap);
void updateEnemyCar(CAR *car, MAP *gameMap);
void resetCar(CAR *car, int initialPosX, int initialPosY);
int checkCarCollision(CAR *car, MAP *map);
int isWall(MAP *map, float x, float y);
void updateLaps(CAR *car, MAP *map);
int isTouchingTile(CAR *car, MAP *map, char targetTile);
