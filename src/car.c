#include "car.h"
#include <math.h>

void drawCar(CAR *car) {
    Rectangle sourceRec = { 0.0f, 0.0f, (float)car->carTexture.width, (float)car->carTexture.height };
    Rectangle destRec = { car->x, car->y, car->carTexture.width, car->carTexture.height };
    Vector2 origin = { car->carTexture.width / 2.0f, car->carTexture.height / 2.0f };

    DrawTexturePro(car->carTexture, sourceRec, destRec, origin, car->angle, WHITE);
    
    // Debug: Visualizar a hitbox física
    // DrawRectangleLinesEx(car->collider, 1, RED);
}

int isWall(MAP *map, float x, float y) {
    int gridX = (int)(x / TILE_SIZE);
    int gridY = (int)(y / TILE_SIZE);

    if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT) return 1;

    char tile = map->tiles[gridY][gridX];
    // Tiles permitidos (Pista) - Lista original
    if (tile == ' ' || tile == 'j' || tile == 'i' || tile == 'l') return 0;

    return 1;
}

int checkCarCollision(CAR *car, MAP *map) {
    // --- RESTAURAÇÃO DA FÍSICA CLÁSSICA ---
    // Voltamos a usar as dimensões FIXAS (60x28) originais do jogo antigo.
    // Ignorar o tamanho da textura evita que texturas novas maiores causem
    // o bug de "travar" na parede. A física volta a ser sólida como antes.
    float useWidth = 60.0f;
    float useHeight = 28.0f;

    float halfW = useWidth / 2.0f;
    float halfH = useHeight / 2.0f;

    float rad = car->angle * DEG2RAD;
    float s = sinf(rad);
    float c = cosf(rad);

    float corners[4][2] = {
        { -halfW, -halfH },
        { +halfW, -halfH },
        { +halfW, +halfH },
        { -halfW, +halfH }
    };

    for (int i = 0; i < 4; i++) {
        float rotatedX = (corners[i][0] * c) - (corners[i][1] * s);
        float rotatedY = (corners[i][0] * s) + (corners[i][1] * c);

        float finalX = car->x + rotatedX;
        float finalY = car->y + rotatedY;

        if (isWall(map, finalX, finalY)) return 1;
    }
    return 0;
}

void updateCar(CAR *car, MAP *gameMap) {
    float deltaTime = GetFrameTime();

    // --- LÓGICA DO NITRO (30% boost) ---
    float baseMaxSpeed = 420.0f;
    float baseAccel = 250.0f;

    if (car->NitroTimer > 0) {
        car->NitroTimer -= deltaTime;
        // Aumenta 30%
        car->maxSpeed = baseMaxSpeed * 1.3f;      // 420 -> 546
        car->acceleration = baseAccel * 1.3f;     // 250 -> 325
    } else {
        car->maxSpeed = baseMaxSpeed;
        car->acceleration = baseAccel;
    }
    
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
    car->collider = (Rectangle){ car->x - 30, car->y - 14,60,28};
}
void updateEnemyCar(CAR *car, MAP *gameMap) {
    float deltaTime = GetFrameTime();

    float baseMaxSpeed = 400.0f;  
    float baseAccel = 200.0f;

    if (car->NitroTimer > 0) {
        car->NitroTimer -= deltaTime;
        car->maxSpeed = baseMaxSpeed * 1.3f;      
        car->acceleration = baseAccel * 1.3f;     
    } else {
        car->maxSpeed = baseMaxSpeed;
        car->acceleration = baseAccel;
    }
    // Limites de velocidade para o inimigo
    if (car->currentSpeed > car->maxSpeed) car->currentSpeed = car->maxSpeed;
    if (car->currentSpeed < -car->maxSpeed * 0.85f) car->currentSpeed = -car->maxSpeed * 0.85f;
    
    // AVISO: O arquivo enemy.c altera o ângulo ANTES de chamar esta função.
    // Isso faz 'oldAngle' já ser o ângulo "perigoso".
    // A lógica abaixo tenta validar, mas se a IA já colocou dentro da parede, não há "oldAngle" seguro aqui.
    float oldAngle = car->angle; 

    car->angle = fmod(car->angle + 360.0f, 360.0f);

    if (checkCarCollision(car, gameMap)) {
        car->angle = oldAngle; // Tenta reverter, mas pode falhar se oldAngle já for inválido
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

    car->y += moveY;
    if(checkCarCollision(car, gameMap)) {
        car->y -= moveY; 
        if(car->currentSpeed > 0) {
            car->currentSpeed -= wallFriction;
            if(car->currentSpeed < 0) car->currentSpeed = 0;
        } else if(car->currentSpeed < 0) {
            car->currentSpeed += wallFriction;
            if(car->currentSpeed > 0) car->currentSpeed = 0;
        }
    }
    
    // CORREÇÃO 3: Atualiza colisor do inimigo (Faltava isso!)
    // Sem isso, tiros e caixas não funcionam no inimigo.
    car->collider = (Rectangle){ car->x - 30, car->y - 14, 60, 28 };
}
    
void updateLaps(CAR *car, MAP *map) {
    float dx = car->x - car->lastLapX;
    float dy = car->y - car->lastLapY;
    float dist = sqrtf(dx*dx + dy*dy);

    if (dist > 2000.0f) car->hasReachedMidpoint = 1;

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

void resetCar(CAR *car, int initialPosX, int initialPosY){
    int wasEnemy = car->isEnemy;

    car->x = (float)initialPosX;
    car->y = (float)initialPosY;
    car->angle = 0.0f;
    
    // Valores do jogo original
    car->width = 60.0f;
    car->height = 28.0f;
    
    car->currentSpeed = 0.0f;
    car->maxSpeed = 420.0f;
    car->acceleration = 250.0f;
    car->brakeSpeed = 300.0f;
    car->friction = 200.0f;
    car->currentLap = 0;
    car->maxLaps = 0;
    car->hasReachedMidpoint = 0;
    car->lastLapX = (float)initialPosX;
    car->lastLapY = (float)initialPosY;
    car->health = 100;
    car->shield = 0;
    car->shieldTimer = 0;
    car->hasShield = 0;
    car->Ammo = 0;
    car->hasBomb = 0;
    car->hasNitro = 0;
    car->NitroTimer = 0;
    car->isEnemy = wasEnemy;
    car->startX = (float)initialPosX;
    car->startY = (float)initialPosY;
    
    // Inicializa collider
    car->collider = (Rectangle){ car->x - 30, car->y - 14, 60, 28 };
}

int checkCollisionPhantom(float x, float y, float angle, MAP *map) {
    float useWidth = 60.0f;
    float useHeight = 28.0f;
    float halfW = useWidth / 2.0f;
    float halfH = useHeight / 2.0f;

    float rad = angle * DEG2RAD;
    float s = sinf(rad);
    float c = cosf(rad);

    float corners[4][2] = {
        { -halfW, -halfH }, { +halfW, -halfH },
        { +halfW, +halfH }, { -halfW, +halfH }
    };

    for (int i = 0; i < 4; i++) {
        float rotatedX = (corners[i][0] * c) - (corners[i][1] * s);
        float rotatedY = (corners[i][0] * s) + (corners[i][1] * c);
        if (isWall(map, x + rotatedX, y + rotatedY)) return 1;
    }
    return 0;
}

// 2. FUNÇÃO PRINCIPAL DE COLISÃO ENTRE CARROS (Adaptada e Corrigida)
void resolveCarToCarCollision(CAR *c1, CAR *c2, MAP *map) {
    float dx = c2->x - c1->x;
    float dy = c2->y - c1->y;
    float dist = sqrtf(dx*dx + dy*dy);
    
    // Raio de colisão ajustado para evitar sobreposições grandes
    float r1 = 28.0f; 
    float r2 = 28.0f;
    float minDist = r1 + r2;

    if (dist < minDist) {
        if (dist == 0) { dx = 1.0f; dist = 1.0f; }

        float nx = dx / dist;
        float ny = dy / dist;
        float overlap = minDist - dist;

        // --- FASE 1: SEPARAÇÃO (Correção de Posição) ---
        // Empurra suavemente para separar, mas VERIFICA PAREDES antes!
        float push = (overlap + 2.0f) * 0.5f; // +2.0f cria um pequeno buffer
        
        // Tenta empurrar C1 para trás
        float c1NewX = c1->x - nx * push;
        float c1NewY = c1->y - ny * push;
        if (!checkCollisionPhantom(c1NewX, c1NewY, c1->angle, map)) {
            c1->x = c1NewX;
            c1->y = c1NewY;
        }

        // Tenta empurrar C2 para frente
        float c2NewX = c2->x + nx * push;
        float c2NewY = c2->y + ny * push;
        if (!checkCollisionPhantom(c2NewX, c2NewY, c2->angle, map)) {
            c2->x = c2NewX;
            c2->y = c2NewY;
        }
        
        // Atualiza colliders após mover
        c1->collider = (Rectangle){ c1->x - 30, c1->y - 14, 60, 28 };
        c2->collider = (Rectangle){ c2->x - 30, c2->y - 14, 60, 28 };

        // --- FASE 2: FÍSICA DE IMPULSO (Troca de Energia) ---
        // Converte velocidade escalar para vetorial
        float rad1 = c1->angle * DEG2RAD;
        float vx1 = cosf(rad1) * c1->currentSpeed;
        float vy1 = sinf(rad1) * c1->currentSpeed;

        float rad2 = c2->angle * DEG2RAD;
        float vx2 = cosf(rad2) * c2->currentSpeed;
        float vy2 = sinf(rad2) * c2->currentSpeed;

        float relVelX = vx2 - vx1;
        float relVelY = vy2 - vy1;
        float velAlongNormal = relVelX * nx + relVelY * ny;

        // Se já estão se afastando, não aplica colisão (evita tremedeira)
        if (velAlongNormal > 0) return;

        // Restituição menor (0.2) para evitar o efeito "bola de pinball"
        float j = -(1.0f + 0.2f) * velAlongNormal;
        j /= 2.0f; // Massas iguais

        float impulseX = j * nx;
        float impulseY = j * ny;

        // Aplica impulso nos vetores
        vx1 -= impulseX;
        vy1 -= impulseY;
        vx2 += impulseX;
        vy2 += impulseY;

        // Reconverte para escalar (Speed) e aplica limites
        c1->currentSpeed = sqrtf(vx1*vx1 + vy1*vy1);
        c2->currentSpeed = sqrtf(vx2*vx2 + vy2*vy2);
        
        // Mantém o sentido correto (frente/ré)
        if ((vx1 * cosf(rad1) + vy1 * sinf(rad1)) < 0) c1->currentSpeed *= -1;
        if ((vx2 * cosf(rad2) + vy2 * sinf(rad2)) < 0) c2->currentSpeed *= -1;

        // CLAMP: Impede velocidades absurdas que fazem atravessar paredes
        if (c1->currentSpeed > c1->maxSpeed) c1->currentSpeed = c1->maxSpeed;
        if (c2->currentSpeed > c2->maxSpeed) c2->currentSpeed = c2->maxSpeed;

        // --- FASE 3: DESVIO ANGULAR ---
        // Faz o carro "escorregar" para o lado se bater de frente
        c1->angle += (float)GetRandomValue(-2, 2);
        c2->angle += (float)GetRandomValue(-2, 2);
    }
}