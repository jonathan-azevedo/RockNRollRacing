#include "car.h"


void drawCar(CAR *car) {
    Vector2 origin = { car->width / 2.0f, car->height / 2.0f };
    DrawRectanglePro((Rectangle){ car->x, car->y, car->width, car->height }, origin, car->angle, RED);
}

void updateCar(CAR *car){
    float deltaTime = GetFrameTime();
    float speed = 350.0;
    float rotationSpeed = 150.0;
    int goFront = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    int goBack = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    int turnLeft = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    int turnRight = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

    if(goFront){
        car->x += cosf(car->angle * DEG2RAD) * speed * deltaTime;
        car->y += sinf(car->angle * DEG2RAD) * speed * deltaTime;
    }
    if(goBack){
        car->x -= cosf(car->angle * DEG2RAD) * (speed * 0.7f) * deltaTime;
        car->y -= sinf(car->angle * DEG2RAD) * (speed * 0.7f) * deltaTime;
    }
    if(turnLeft){
        if(goFront){
            car->angle -= rotationSpeed * deltaTime;
        } else if(goBack){
            car->angle += rotationSpeed * deltaTime;
        }
    }
    if(turnRight){
        if(goFront){
            car->angle += rotationSpeed * deltaTime;
        } else if(goBack){
            car->angle -= rotationSpeed * deltaTime;
        }
    }
}