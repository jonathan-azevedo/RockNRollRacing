#pragma once
#include <raylib.h>
#include <math.h>



typedef struct{
        float x;
        float y;
        float angle;
        float width;
        float height;
    }CAR;


void drawCar(CAR *car);
void updateCar(CAR *car);