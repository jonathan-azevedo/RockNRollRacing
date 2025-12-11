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

    // fisica
    float currentSpeed;
    float maxSpeed;
    float acceleration;
    float brakeSpeed;
    float friction;

    // gameplay
    int currentLap;
    int maxLaps;
    int hasReachedMidpoint;
    float lastLapX;
    float lastLapY;

    // status
    int health;
    int shield;
    float shieldTimer;
    int hasShield;
    int Ammo;
    int hasBomb;
    int hasNitro;
    float NitroTimer;

    // flags
    int isEnemy;
    Rectangle collider; //hitbox para deteccao de tiros/powerups

    // respawn
    float startX;
    float startY;

} CAR;

void drawCar(CAR *car); //desenha o sprite do carro rotacionado na tela
void updateCar(CAR *car, MAP *gameMap); // loop de fisica do jogador
void updateEnemyCar(CAR *car, MAP *gameMap); // loop de fisica do inimigo
void resetCar(CAR *car, int initialPosX, int initialPosY); // reseta o carro para a linha de largada e reseta status iniciais
int checkCarCollision(CAR *car, MAP *map); // detecta colisao do carro com as paredes do mapa
int isWall(MAP *map, float x, float y); // verifica se uma coordenada é uma parede
void updateLaps(CAR *car, MAP *map); //gerencia contagem de voltas e checkpoints
int isTouchingTile(CAR *car, MAP *map, char targetTile); //verifica se o carro esta tocando um tipo especifico de tile
void resolveCarToCarCollision(CAR *c1, CAR *c2, MAP *map);//resolve a batida entre dois carros (empurrao+ricochete)
int checkCollisionPhantom(float x, float y, float angle, MAP *map); // Função auxiliar para prever colisão antes de mover