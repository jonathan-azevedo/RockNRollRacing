#include "itens.h"
#include <stdlib.h>
#include <math.h>

void setRandomSpawnPosition(LOOT_BOX *item, MAP *map) {
    //encontra uma posicao valida na matriz para spawnar itens
    int valid = 0;
    while(!valid) {
        int gridX = GetRandomValue(1, MAP_WIDTH - 2);
        int gridY = GetRandomValue(1, MAP_HEIGHT - 2);
        char tile = map->tiles[gridY][gridX];
        int isWall = (tile == 'p' || tile == 'P');
        int isTrack = (tile == ' ' || tile == 'i' || tile == 'j' || tile == 'l');
        if (!isWall && isTrack) {
            float offset = (TILE_SIZE - 40) / 2.0f;
            item->rect.x = (gridX * TILE_SIZE) + offset;
            item->rect.y = (gridY * TILE_SIZE) + offset;
            item->rect.width = 40;
            item->rect.height = 40;
            valid = 1;
        }
    }
}

void initItems(ITEM_MANAGER *manager, MAP *map) {
    //posiciona as caixas de itens iniciais no mapa
    for(int i=0; i<MAX_BOMBS; i++) manager->bombs[i].active = false;
    for(int i=0; i<MAX_PROJECTILES; i++) manager->projectiles[i].active = false;
    for(int i=0; i<MAX_EFFECTS; i++) manager->effects[i].active = false;
    for(int i = 0; i < MAX_ITEMS; i++) {
        manager->items[i].active = true;
        manager->items[i].respawnTimer = 0;
        setRandomSpawnPosition(&manager->items[i], map);
    }
}

void spawnExplosion(ITEM_MANAGER *manager, float x, float y) {
    //inicia uma animação visual de explosão
    for(int i=0; i<MAX_EFFECTS; i++) {
        if(!manager->effects[i].active) {
            manager->effects[i].active = true;
            manager->effects[i].x = x;
            manager->effects[i].y = y;
            manager->effects[i].maxTime = 0.5f;
            manager->effects[i].timer = manager->effects[i].maxTime;
            break;
        }
    }
}

void spawnProjectile(ITEM_MANAGER *manager, float x, float y, float angle, bool isPlayer) {
    //dispara um projetil
    for(int i=0; i<MAX_PROJECTILES; i++) {
        if(!manager->projectiles[i].active) {
            manager->projectiles[i].active = true;
            manager->projectiles[i].x = x;
            manager->projectiles[i].y = y;
            manager->projectiles[i].angle = angle;
            manager->projectiles[i].firedByPlayer = isPlayer;
            float speed = 900.0f;
            manager->projectiles[i].dx = cosf(angle * DEG2RAD) * speed;
            manager->projectiles[i].dy = sinf(angle * DEG2RAD) * speed;
            break;
        }
    }
}

void spawnBomb(ITEM_MANAGER *manager, float x, float y, int ID) {
    //coloca uma bomba no chão e define um tempo de segurança para que nao exploda no dono instantaneamente
    for(int i=0; i<MAX_BOMBS; i++) {
        if(!manager->bombs[i].active) {
            manager->bombs[i].active = true;
            manager->bombs[i].x = x;
            manager->bombs[i].y = y;
            manager->bombs[i].ID = ID; 
            manager->bombs[i].safeTimer = 2.0f;
            break;
        }
    }
}

void applyLoot(CAR *car, ITEM_MANAGER *manager) {
    //decide qual item vai ganhar
    int r = GetRandomValue(0, 100);
    car->Ammo = 0; 
    car->hasBomb = 0;
    car->hasShield = 0;
    car->hasNitro = 0; 

    // Probabilidades
    if (r < 15) {
        car->hasShield = 1;
    } else if (r < 50) {
        car->Ammo = 1;
    } else if (r < 75) {
        car->hasBomb = 1;
    } else {
        car->hasNitro = 1;
    }
}

void updateItems(ITEM_MANAGER *manager, CAR *player, ENEMY enemies[], int enemyCount, float dt, GameAudio *audio, MAP *map) {
    // caixas de loot
    for(int i=0; i<MAX_ITEMS; i++) {
        if(manager->items[i].active) {
            // colisão com player
            if(CheckCollisionRecs(player->collider, manager->items[i].rect)) {
                
                if (player->Ammo == 0 && player->hasBomb == 0 && player->hasShield == 0 && player->hasNitro == 0) {
                    applyLoot(player, manager);
                    manager->items[i].active = false;
                    manager->items[i].respawnTimer = 1.0f;
                }
            }
            else {
                // colisão com Inimigos
                for (int e = 0; e < enemyCount; e++) {
                    if (CheckCollisionRecs(enemies[e].vehicle.collider, manager->items[i].rect)) {
                        applyLoot(&enemies[e].vehicle, manager);
                        
                        // IA usa itens imediatamente
                        if (enemies[e].vehicle.hasBomb) {
                            spawnBomb(manager, enemies[e].vehicle.x, enemies[e].vehicle.y, e); 
                            enemies[e].vehicle.hasBomb = 0;
                        }
                        if (enemies[e].vehicle.Ammo) {
                            spawnProjectile(manager, enemies[e].vehicle.x, enemies[e].vehicle.y, enemies[e].vehicle.angle, false);
                            enemies[e].vehicle.Ammo = 0;
                            PlaySound(audio->shoot);
                        }
                        if (enemies[e].vehicle.hasShield) { 
                            enemies[e].vehicle.hasShield = 0;
                            enemies[e].vehicle.shieldTimer = 20.0f; 
                        }
                        if (enemies[e].vehicle.hasNitro) {
                            enemies[e].vehicle.hasNitro = 0;
                            enemies[e].vehicle.NitroTimer = 5.0f;
                        }

                        manager->items[i].active = false;
                        manager->items[i].respawnTimer = 1.0f;
                        break;
                    }
                }
            }
        } else {
            manager->items[i].respawnTimer -= dt;
            if(manager->items[i].respawnTimer <= 0) {
                manager->items[i].active = true;
                setRandomSpawnPosition(&manager->items[i], map);
            }
        }
    }

    // bombas
    for(int i = 0; i < MAX_BOMBS; i++) {
        if(manager->bombs[i].active) {
            if(manager->bombs[i].safeTimer > 0) manager->bombs[i].safeTimer -= dt; 
            if(!manager->bombs[i].active) continue;

            Rectangle bombR = {manager->bombs[i].x, manager->bombs[i].y, 32, 32};
            bool exploded = false;

            // colisao com player
            if (CheckCollisionRecs(player->collider, bombR)) {
                bool isSafe = (manager->bombs[i].ID == -1 && manager->bombs[i].safeTimer > 0);
                
                if (!isSafe) {
                    exploded = true;
                    spawnExplosion(manager, manager->bombs[i].x, manager->bombs[i].y);
                    PlaySound(audio->damage);
                    if (player->shieldTimer <= 0) player->health -= 50;
                    player->currentSpeed = 0; 
                }
            }

            // colisao com inimigos
            if (!exploded) {
                for (int e = 0; e < enemyCount; e++) {
                    if (CheckCollisionRecs(enemies[e].vehicle.collider, bombR)) {
                        bool isSafe = (manager->bombs[i].ID == e && manager->bombs[i].safeTimer > 0);
                        
                        if (!isSafe) {
                            exploded = true;
                            spawnExplosion(manager, manager->bombs[i].x, manager->bombs[i].y);
                            PlaySound(audio->damage);
                            if (enemies[e].vehicle.shieldTimer <= 0) enemies[e].vehicle.health -= 50;
                            enemies[e].vehicle.currentSpeed = 0;
                            break; 
                        }
                    }
                }
            }

            if (exploded) manager->bombs[i].active = false;
        }
    }

    // projeteis
    for(int i=0; i<MAX_PROJECTILES; i++) {
        if(manager->projectiles[i].active) {
            manager->projectiles[i].x += manager->projectiles[i].dx * dt;
            manager->projectiles[i].y += manager->projectiles[i].dy * dt;
            
            if (manager->projectiles[i].x < 0 || manager->projectiles[i].x > MAP_WIDTH * TILE_SIZE ||
                manager->projectiles[i].y < 0 || manager->projectiles[i].y > MAP_HEIGHT * TILE_SIZE) {
                manager->projectiles[i].active = false;
                continue;
            }

            Vector2 center = {manager->projectiles[i].x, manager->projectiles[i].y};

            if (manager->projectiles[i].firedByPlayer) {
                for (int e = 0; e < enemyCount; e++) {
                    if (CheckCollisionCircleRec(center, 15, enemies[e].vehicle.collider)) {
                        manager->projectiles[i].active = false;
                        spawnExplosion(manager, center.x, center.y);
                        PlaySound(audio->damage);
                        
                        if (enemies[e].vehicle.shieldTimer > 0) {
                        } else {
                            enemies[e].vehicle.health -= 50;
                            enemies[e].vehicle.currentSpeed *= 0.2f; 
                        }
                        break;
                    }
                }
            } 
            
            else {
                if (CheckCollisionCircleRec(center, 15, player->collider)) {
                    manager->projectiles[i].active = false;
                    spawnExplosion(manager, center.x, center.y);
                    PlaySound(audio->damage);
                    
                    if (player->shieldTimer > 0) {
                        
                    } else {
                        player->health -= 50; 
                        player->currentSpeed *= 0.2f; 
                    }
                }
            }
        }
    }

    // efeitos
    for(int i=0; i<MAX_EFFECTS; i++) {
        if(manager->effects[i].active) {
            manager->effects[i].timer -= dt;
            if(manager->effects[i].timer <= 0) manager->effects[i].active = false;
        }
    }
}

void drawItems(ITEM_MANAGER *manager, GAME_TEXTURES *textures) {
    //renderiza todos os objetos ativos, incluindo a animação da explosão
    for(int i=0; i<MAX_ITEMS; i++) {
        if(manager->items[i].active) {
            if(textures->luckBox.id > 0) DrawTexturePro(textures->luckBox, (Rectangle){0,0, textures->luckBox.width, textures->luckBox.height}, manager->items[i].rect, (Vector2){0,0}, 0.0f, WHITE);
            else DrawRectangleRec(manager->items[i].rect, GOLD);
        }
    }
    for(int i=0; i<MAX_BOMBS; i++) {
        if(manager->bombs[i].active) {
             if(textures->bomb.id > 0) DrawTexturePro(textures->bomb, (Rectangle){0,0, textures->bomb.width, textures->bomb.height}, (Rectangle){manager->bombs[i].x, manager->bombs[i].y, 32, 32}, (Vector2){0,0}, 0.0f, WHITE);
             else DrawCircle((int)manager->bombs[i].x+15, (int)manager->bombs[i].y+15, 10, RED);
        }
    }
    for(int i=0; i<MAX_PROJECTILES; i++) {
        if(manager->projectiles[i].active) {
            if (textures->projectile.id > 0) {
                Rectangle source = {0,0, textures->projectile.width, textures->projectile.height};
                Rectangle dest = {manager->projectiles[i].x, manager->projectiles[i].y, 16, 32};
                DrawTexturePro(textures->projectile, source, dest, (Vector2){8,16}, manager->projectiles[i].angle + 90, WHITE);
            } else DrawCircle((int)manager->projectiles[i].x, (int)manager->projectiles[i].y, 5, YELLOW);
        }
    }
    for(int i=0; i<MAX_EFFECTS; i++) {
        if(manager->effects[i].active) {
            if (textures->explosion.id > 0) {
                float lifePerc = 1.0f - (manager->effects[i].timer / manager->effects[i].maxTime);
                int frame = (int)(lifePerc * 5); if(frame >= 5) frame = 4;
                float frameW = (float)textures->explosion.width / 5;
                Rectangle source = {frame * frameW, 0, frameW, (float)textures->explosion.height};
                Rectangle dest = {manager->effects[i].x, manager->effects[i].y, 64, 64};
                DrawTexturePro(textures->explosion, source, dest, (Vector2){32,32}, 0.0f, WHITE);
            } else DrawCircle((int)manager->effects[i].x, (int)manager->effects[i].y, 20, ORANGE);
        }
    }
}