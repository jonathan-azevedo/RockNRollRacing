#pragma once
#include <raylib.h>


// armazena todos os assets sonoros do jogo
typedef struct {
    Music menuMusic;      // Skillet - Monster
    Music raceMusic;      // Heavy Battle

    Sound countdown;
    Sound shoot;
    Sound damage;
} GameAudio;


GameAudio LoadGameAudio();              // Inicializa o sistema de áudio, carrega arquivos e configura volumes padrão
void UnloadGameAudio(GameAudio *audio); // Libera memória de todos os assets 
void UpdateGameMusic(GameAudio *audio, int gameState); // Gerencia troca de músicas baseada no estado do jogo e atualiza os buffers de música a cada frame.
