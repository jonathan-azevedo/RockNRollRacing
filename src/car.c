#include "car.h"


void drawCar(CAR *car) {
    
    Rectangle sourceRec = { 0.0f, 0.0f, (float)car->carTexture.width, (float)car->carTexture.height };
    Rectangle destRec = { car->x, car->y, car->width, car->height };
    Vector2 origin = { car->width / 2.0f, car->height / 2.0f };

    DrawTexturePro(car->carTexture, sourceRec, destRec, origin, car->angle, WHITE);


}

void updateCar(CAR *car){
    float deltaTime = GetFrameTime();
    float speed = 400.0;
    float rotationSpeed = 150.0;
    int goFront = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    int goBack = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    int turnLeft = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    int turnRight = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

    if(goFront) {
        car->currentSpeed += car->acceleration * deltaTime;
        if (car->currentSpeed < 0) {
            car->currentSpeed += car->brakeSpeed * deltaTime;
        }
    } 
    else if(goBack) {
        car->currentSpeed -= car->acceleration * 0.85f * deltaTime; 
        if (car->currentSpeed > 0) {
            car->currentSpeed -= car->brakeSpeed * deltaTime;
        }
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

    if (car->currentSpeed > car->maxSpeed) 
        car->currentSpeed = car->maxSpeed;
    if (car->currentSpeed < -car->maxSpeed * 0.85f) 
        car->currentSpeed = -car->maxSpeed * 0.85f; 
    
    if (fabs(car->currentSpeed) > 0.1f || goFront || goBack) {
        if(turnLeft){
            car->angle -= (car->currentSpeed * 0.52f) * deltaTime;
        }
        if(turnRight){
            car->angle += (car->currentSpeed * 0.52f) * deltaTime;
        }
    }

    car->x += cosf(car->angle * DEG2RAD) * car->currentSpeed * deltaTime;
    car->y += sinf(car->angle * DEG2RAD) * car->currentSpeed * deltaTime;
    car->angle = fmod(car->angle + 360.0f, 360.0f);



}
