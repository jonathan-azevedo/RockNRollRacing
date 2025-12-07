#include "enemy.h"
#include <math.h>
#include <stdlib.h>

void initEnemy(ENEMY *enemy, MAP *map, Texture2D texture, int offsetIndex) {
    int startX = 200, startY = 200;

    // Vari�vel para contar quantos 'i' j� encontramos no mapa
    int iFoundCount = 0;
    int positionFound = 0;

    // Varre o mapa procurando a posi��o correta
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if(map->tiles[y][x] == 'i'){

                // Se o �ndice deste 'i' bater com o �ndice do inimigo (0 ou 1)
                if (iFoundCount == offsetIndex) {
                    startX = x * TILE_SIZE + TILE_SIZE/2;
                    startY = y * TILE_SIZE + TILE_SIZE/2;
                    positionFound = 1;
                    break; // Achamos a posi��o deste inimigo espec�fico!
                }

                // Se n�o for este, incrementa e continua procurando o pr�ximo 'i'
                iFoundCount++;
            }
        }
        if (positionFound) break;
    }

    // Se n�o achou 'i' suficiente no mapa para todos os inimigos (ex: tem 2 inimigos mas s� 1 'i')
    // A� sim usamos a l�gica de fila indiana como fallback (plano B)
    if (!positionFound && offsetIndex > 0) {
        // Pega a posi��o do inimigo anterior e recua
        // Nota: Isso � raro acontecer se o mapa estiver bem feito
        startX -= (60 * offsetIndex);
    }

    resetCar(&enemy->vehicle, startX, startY);

    enemy->vehicle.carTexture = texture;
    enemy->vehicle.maxSpeed = 380.0f;
    enemy->vehicle.acceleration = 180.0f;
    enemy->vehicle.isEnemy = 1;
    enemy->id = offsetIndex;

    enemy->turnTimer = 0.0f;
    enemy->turnDuration = 0.0f;
    enemy->turnDirection = 0;

    // Varia��o de velocidade
    if (offsetIndex % 2 != 0) enemy->vehicle.maxSpeed += 15.0f;
}

// ... (O resto do arquivo continua IGUAL: checkEnemyWall, updateEnemy, drawEnemy) ...
int checkEnemyWall(MAP *map, float x, float y) {
    return isWall(map, x, y);
}

void updateEnemy(ENEMY *enemy, MAP *map) {
    float dt = GetFrameTime();
    CAR *c = &enemy->vehicle;

    // 1. Ler sensores
    // Olha 30 graus para cada lado
    float distLeft  = getWallDistance(map, c->x, c->y, c->angle - 30.0f); 
    float distRight = getWallDistance(map, c->x, c->y, c->angle + 30.0f);
    float distFront = getWallDistance(map, c->x, c->y, c->angle);

    // 2. Lógica de Direção (Proporcional)
    // Se a diferença for grande, vira mais forte.
    // Raylib: +Ângulo é Horário (Direita), -Ângulo é Anti-horário (Esquerda)
    
    // Se Right > Left, queremos virar à Direita (+). 
    // Se Left > Right, queremos virar à Esquerda (-).
    float turnForce = (distRight - distLeft) * 2.0f * dt; 
    
    // Aplica a curva suavemente para centralizar na pista
    c->angle += turnForce;

    // 3. Prevenção de Colisão Frontal (Freio de Emergência)
    float speedTarget = c->maxSpeed;
    
    if (distFront < 150.0f) {
        // Se tem parede logo à frente, freia e curva mais forte para o lado mais livre
        speedTarget = 50.0f; 
        float emergencyTurn = (distRight > distLeft ? 150.0f : -150.0f) * dt;
        c->angle += emergencyTurn;
    }

    // 4. Controle de Aceleração Simples
    if (c->currentSpeed < speedTarget) {
        c->currentSpeed += c->acceleration * dt;
    } else {
        c->currentSpeed -= c->friction * dt; // Solta o acelerador
    }

    // Mecânica de Ré (caso trave)
    if (c->currentSpeed < 10.0f && distFront < 40.0f) {
        enemy->turnTimer += dt;
        if (enemy->turnTimer > 1.0f) {
             c->currentSpeed = -100.0f; // Ré
             // Enquanto dá ré, inverte a direção para sair do enrosco
             c->angle += (distRight > distLeft ? -100.0f : 100.0f) * dt; 
        }
    } else {
        enemy->turnTimer = 0.0f;
    }
    
    // Atualiza física (Lembre-se de corrigir o bug da rotação antes de chamar isso,
    // mas com essa lógica suave, o bug de colisão deve diminuir drasticamente)
    updateEnemyCar(c, map);
    updateLaps(c, map);
}

void drawEnemy(ENEMY *enemy) {
    drawCar(&enemy->vehicle);
}
float getWallDistance(MAP *map, float startX, float startY, float angleDeg) {
    float rad = angleDeg * DEG2RAD;
    float dirX = cosf(rad);
    float dirY = sinf(rad);
    
    float dist = 0;
    float step = 10.0f; // Precisão do raio (quanto menor, mais preciso/pesado)
    float maxDist = 500.0f; // Visão máxima da IA

    while (dist < maxDist) {
        float checkX = startX + dirX * dist;
        float checkY = startY + dirY * dist;
        
        // Se bater na parede, retorna a distância atual
        if (isWall(map, checkX, checkY)) {
            return dist;
        }
        dist += step;
    }
    return maxDist; // Caminho livre
}
