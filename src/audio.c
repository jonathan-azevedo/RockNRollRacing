#include "audio.h"
#include <stdio.h>

// Definição dos estados copiados do main para referência interna
// MENU=0, GAME_SETUP=1, PLAYING=2, VICTORY=3, PAUSE=4

typedef enum {MENU, GAME_SETUP, PLAYING, VICTORY, PAUSE, COUNTDOWN} State;

GameAudio LoadGameAudio() {
    GameAudio audio = {0};

    // --- CARREGANDO MÚSICAS ---
    // Ajuste os volumes aqui (0.0f a 1.0f)
    
    audio.menuMusic = LoadMusicStream("Assets/Sounds/Skillet-Monster.mp3");
    SetMusicVolume(audio.menuMusic, 0.5f);
    // Faz a música do menu loopar corretamente se tiver metadados de loop
    audio.menuMusic.looping = true; 

    audio.raceMusic = LoadMusicStream("Assets/Sounds/heavy_battle.mp3");
    SetMusicVolume(audio.raceMusic, 0.4f);
    audio.raceMusic.looping = true;

    // --- CARREGANDO EFEITOS SONOROS ---
    audio.countdown = LoadSound("Assets/Sounds/countdown.ogg");
    audio.damage = LoadSound("Assets/Sounds/damage.mp3");
    audio.shoot = LoadSound("Assets/Sounds/shoot.mp3");

    return audio;
}

void UnloadGameAudio(GameAudio *audio) {
    UnloadMusicStream(audio->menuMusic);
    UnloadMusicStream(audio->raceMusic);
    
    UnloadSound(audio->countdown);
    UnloadSound(audio->shoot);
    UnloadSound(audio->damage);
}

void UpdateGameMusic(GameAudio *audio, int gameState) {
    // Variável estática para detectar mudança de estado
    static State lastState = -1; 

    if (gameState != lastState) {
        
        // --- TRANSIÇÃO: ENTRANDO EM PAUSE ---
        if (gameState == PAUSE) { 
            PauseMusicStream(audio->raceMusic);
            PauseMusicStream(audio->menuMusic);
        }
        
        // --- TRANSIÇÃO: ENTRANDO NO JOGO OU COUNTDOWN ---
        else if (gameState == PLAYING || gameState == COUNTDOWN) {
            StopMusicStream(audio->menuMusic); 
            
            if (lastState == PAUSE) { 
                ResumeMusicStream(audio->raceMusic);
            } else { 
                // Se veio do menu, reinicia a música
                StopMusicStream(audio->raceMusic);
                PlayMusicStream(audio->raceMusic);
            }
        }
        
        // --- TRANSIÇÃO: INDO PARA MENUS ---
        else if (gameState == MENU || gameState == GAME_SETUP || gameState == VICTORY) {
            StopMusicStream(audio->raceMusic);
            
            if (lastState == PAUSE) {
                ResumeMusicStream(audio->menuMusic);
            } else {
                if (!IsMusicStreamPlaying(audio->menuMusic)) 
                    PlayMusicStream(audio->menuMusic);
            }
        }
        
        lastState = gameState;
    }

    // --- LOOP DE ATUALIZAÇÃO ---
    // Mantém a música rodando
    if (gameState == MENU || gameState == GAME_SETUP || gameState == VICTORY) {
        UpdateMusicStream(audio->menuMusic);
    } 
    else if (gameState == PLAYING || gameState == COUNTDOWN) {
        UpdateMusicStream(audio->raceMusic);
    }
}