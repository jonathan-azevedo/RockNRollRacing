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
    float currentSpeed;  
    float maxSpeed;
    float acceleration;
    float brakeSpeed;       
    float friction;  
    int currentLap;
    int maxLaps;
    int hasReachedMidpoint; 
    float lastLapX;           
    float lastLapY; 
    int health;      
    int shield;      
    int Ammo;    
    }CAR;


void drawCar(CAR *car);
void updateCar(CAR *car, MAP *gameMap);
void resetCar(CAR *car, int initialPosX, int initialPosY);
int checkCarCollision(CAR *car, MAP *map);
int isWall(MAP *map, float x, float y);
void updateLaps(CAR *car, MAP *map);
int isTouchingTile(CAR *car, MAP *map, char targetTile);