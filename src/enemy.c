#include "enemy.h"
#include <math.h>
#include <stdlib.h>


void initEnemy(ENEMY *enemy, MAP *map, Texture2D texture, int offsetIndex) {
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
    enemy->turnDuration = 0.0f; // Usaremos isso para controlar o tempo da ré
    enemy->turnDirection = 0;

    if (offsetIndex % 2 != 0) enemy->vehicle.maxSpeed += 15.0f;
}

int checkEnemyWall(MAP *map, float x, float y) {
    return isWall(map, x, y);
}

float getWallDistance(MAP *map, float startX, float startY, float angleDeg) {
    float rad = angleDeg * DEG2RAD;
    float dirX = cosf(rad);
    float dirY = sinf(rad);
    
    float dist = 0;
    float step = 15.0f; // Otimizado: passos maiores para performance
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

    // --- MÁQUINA DE ESTADOS: RECUPERAÇÃO (RÉ) ---
    // Se o timer for negativo, estamos executando a manobra de ré
    if (enemy->turnDuration > 0.0f) {
        enemy->turnDuration -= dt;
        
        c->currentSpeed = -120.0f; // Força ré
        
        // Gira para o lado que estava mais livre quando travou
        float dir = (enemy->turnDirection == 1) ? 1.0f : -1.0f;
        c->angle += (130.0f * dir) * dt;

        updateEnemyCar(c, map);
        updateLaps(c, map);
        return; // Sai da função para ignorar a IA normal
    }

    // --- IA NORMAL ---

    // 1. Sensores
    float distLeft  = getWallDistance(map, c->x, c->y, c->angle - 35.0f); // Ângulo um pouco mais aberto (35)
    float distRight = getWallDistance(map, c->x, c->y, c->angle + 35.0f);
    float distFront = getWallDistance(map, c->x, c->y, c->angle);

    // 2. Lógica de Direção com VIÉS DE FAIXA (Lane Bias)
    // Se ID é par (0, 2), prefere direita (+40). Ímpar (1, 3) prefere esquerda (-40).
    // Isso faz com que, numa reta, um ande de um lado e outro do outro.
    float laneBias = (enemy->id % 2 == 0) ? 40.0f : -40.0f;
    
    // Adicionamos o bias à diferença. O carro "acha" que a parede está mais perto de um lado
    float turnForce = ((distRight - distLeft) + laneBias) * 2.5f * dt; 
    c->angle += turnForce;

    // 3. Controle de Velocidade e Freio
    float speedTarget = c->maxSpeed;

    // Curvas fechadas -> Reduz velocidade
    if (fabsf(distRight - distLeft) > 150.0f) {
        speedTarget = c->maxSpeed * 0.70f;
    }

    // Parede na frente -> Freio de emergência e curva forte
    if (distFront < 140.0f) {
        speedTarget = 50.0f; 
        // Curva de pânico: ignora o viés e vai pro lado mais livre
        float emergencyTurn = (distRight > distLeft ? 180.0f : -180.0f) * dt;
        c->angle += emergencyTurn;
    }

    // Aplica Aceleração
    if (c->currentSpeed < speedTarget) {
        c->currentSpeed += c->acceleration * dt;
    } else {
        c->currentSpeed -= c->friction * dt;
    }

    // 4. DETECTOR DE TRAVAMENTO INTELIGENTE
    // Se a velocidade for muito baixa (< 15) mas o carro não estiver dando ré intencionalmente
    if (c->currentSpeed > -20.0f && c->currentSpeed < 15.0f) {
        enemy->turnTimer += dt;
        
        // Se ficar preso por 1.0 segundo (seja parede ou outro carro)
        if (enemy->turnTimer > 1.0f) {
             enemy->turnDuration = 1.2f; // Configura 1.2s de ré
             enemy->turnTimer = 0.0f;    // Reseta contador
             
             // Decide para onde girar na ré (lado mais livre)
             enemy->turnDirection = (distRight > distLeft) ? 1 : 0;
        }
    } else {
        enemy->turnTimer = 0.0f; // Reseta se estiver andando bem
    }
    
    updateEnemyCar(c, map);
    updateLaps(c, map);
}

void drawEnemy(ENEMY *enemy) {
    drawCar(&enemy->vehicle);
}