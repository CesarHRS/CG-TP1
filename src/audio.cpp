#include "audio.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

// Detectar se estamos no Linux e se SDL2 está disponível
#ifdef __linux__
    #define USE_SDL2
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_mixer.h>
#endif

#ifdef USE_SDL2
// Implementação completa com SDL2 para Linux
Audio::Audio()
    : initialized(false)
{
    for (int i = 0; i < SOUND_COUNT; ++i) chunks[i] = nullptr;
}

Audio::~Audio() {
    cleanup();
}

Audio& Audio::getInstance() {
    static Audio inst;
    return inst;
}

bool Audio::init() {
    if (initialized) return true;
    
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Mix_OpenAudio failed: %s\n", Mix_GetError());
        SDL_Quit();
        return false;
    }
    
    initialized = true;
    return true;
}

void Audio::loadAll() {
    if (!initialized) {
        if (!init()) return;
    }
    
    const char* paths[SOUND_COUNT] = {
        "assets/sounds/correct.wav",
        "assets/sounds/error.wav",
        "assets/sounds/victory.wav",
        "assets/sounds/shoot.wav"
    };
    
    for (int i = 0; i < SOUND_COUNT; ++i) {
        chunks[i] = Mix_LoadWAV(paths[i]);
        if (!chunks[i]) {
            printf("Failed to load sound %s: %s\n", paths[i], Mix_GetError());
        }
    }
}

void Audio::play(SoundId id) {
    if (!initialized || id < 0 || id >= SOUND_COUNT) return;
    if (chunks[id]) {
        Mix_PlayChannel(-1, chunks[id], 0);
    }
}

void Audio::cleanup() {
    if (!initialized) return;
    
    for (int i = 0; i < SOUND_COUNT; ++i) {
        if (chunks[i]) {
            Mix_FreeChunk(chunks[i]);
            chunks[i] = nullptr;
        }
    }
    
    Mix_CloseAudio();
    SDL_Quit();
    initialized = false;
}

#else
// Implementação stub para Windows (sem SDL2)
Audio::Audio()
    : initialized(false)
{
    for (int i = 0; i < SOUND_COUNT; ++i) chunks[i] = nullptr;
}

Audio::~Audio() {
    cleanup();
}

Audio& Audio::getInstance() {
    static Audio inst;
    return inst;
}

bool Audio::init() {
    initialized = true;
    return true;
}

void Audio::loadAll() {
    if (!initialized) {
        init();
    }
}

void Audio::play(SoundId id) {
    (void)id;
}

void Audio::cleanup() {
    initialized = false;
}
#endif

extern "C" void audio_cleanup_at_exit() {
    Audio::getInstance().cleanup();
}
