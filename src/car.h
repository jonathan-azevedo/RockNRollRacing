#pragma once
#include <raylib.h>
#include <math.h>


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
    }CAR;


void drawCar(CAR *car);
void updateCar(CAR *car);