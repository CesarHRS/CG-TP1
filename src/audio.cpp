#include "audio.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

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
        std::cerr << "SDL_Init(SDL_INIT_AUDIO) failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Open audio: 44100 Hz, default format, stereo, 2048 chunk size
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Mix_OpenAudio failed: " << Mix_GetError() << std::endl;
        return false;
    }

    // Initialize support for different formats (optional)
    int flags = MIX_INIT_MP3 | MIX_INIT_OGG;
    int initted = Mix_Init(flags);
    (void)initted;

    initialized = true;
    return true;
}

static Mix_Chunk* loadChunk(const char* path) {
    Mix_Chunk* c = Mix_LoadWAV(path);
    if (!c) {
        std::cerr << "Warning: failed to load sound '" << path << "': " << Mix_GetError() << std::endl;
    }
    return c;
}

void Audio::loadAll() {
    if (!initialized) {
        // try to init; if fails, we keep going but won't play sounds
        if (!init()) return;
    }

    // Filenames used by the game. Place files at these paths.
    const char* names[SOUND_COUNT] = {
        "assets/sounds/laser.mp3",      // SOUND_LASER
        "assets/sounds/explosion.mp3",  // SOUND_EXPLOSION
        "assets/sounds/error.mp3",     // SOUND_DAMAGE
        "assets/sounds/fail.mp3",      // SOUND_ERROR
        "assets/sounds/victory.mp3"     // SOUND_VICTORY
    };

    for (int i = 0; i < SOUND_COUNT; ++i) {
        if (chunks[i]) continue;
        Mix_Chunk* c = loadChunk(names[i]);
        chunks[i] = (void*)c;
    }
}

void Audio::play(SoundId id) {
    if (!initialized) return;
    if (id < 0 || id >= SOUND_COUNT) return;
    Mix_Chunk* c = (Mix_Chunk*)chunks[id];
    if (!c) return;
    // channel -1 (use first free), no loops
    if (Mix_PlayChannel(-1, c, 0) == -1) {
        std::cerr << "Mix_PlayChannel failed: " << Mix_GetError() << std::endl;
    }
}

void Audio::cleanup() {
    if (!initialized) return;

    for (int i = 0; i < SOUND_COUNT; ++i) {
        Mix_Chunk* c = (Mix_Chunk*)chunks[i];
        if (c) Mix_FreeChunk(c);
        chunks[i] = nullptr;
    }

    Mix_CloseAudio();
    Mix_Quit();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    SDL_Quit();
    initialized = false;
}

extern "C" void audio_cleanup_at_exit() {
    Audio::getInstance().cleanup();
}
