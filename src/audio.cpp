#include "audio.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>

// Try to enable SDL2 / SDL2_mixer if headers are available. If not,
// the code falls back to the previous stub behavior.
#if defined(__has_include)
# if __has_include(<SDL2/SDL.h>) && __has_include(<SDL2/SDL_mixer.h>)
#  include <SDL2/SDL.h>
#  include <SDL2/SDL_mixer.h>
#  define AUDIO_HAVE_SDL 1
# endif
#endif

// Opaque pointer to Mix_Music stored as void* so header doesn't need SDL.
static void* s_music = nullptr;

Audio::Audio()
    : initialized(false)
{
    for (int i = 0; i < SOUND_COUNT; ++i) chunks[i] = nullptr;
    currentMusicPath = "";
    musicPlaying = false;
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
#ifdef AUDIO_HAVE_SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "[Audio] SDL_Init failed: " << SDL_GetError() << "\n";
        // fall through to stub behavior
    } else {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            std::cerr << "[Audio] Mix_OpenAudio failed: " << Mix_GetError() << "\n";
        }
    }
#endif
    initialized = true;
    return true;
}

void Audio::loadAll() {
    if (!initialized) init();
    // For now we don't eagerly load music/effects; playMusic will load on demand.
}

void Audio::play(SoundId id) {
    if (!initialized) init();
#ifdef AUDIO_HAVE_SDL
    const char *soundPaths[] = {
        "assets/sounds/laser.mp3",
        "assets/sounds/explosion.mp3",
        "assets/sounds/error.mp3",
        "assets/sounds/fail.mp3",
        "assets/sounds/victory.mp3"
    };
    
    if (id < 0 || id >= SOUND_COUNT) return;
    
    const char *path = soundPaths[id];
    Mix_Chunk *chunk = Mix_LoadWAV(path);
    if (!chunk) {
        // Try loading with different format if WAV fails
        chunk = (Mix_Chunk*)Mix_LoadMUS(path);
        if (!chunk) {
            std::cerr << "[Audio] Mix_LoadWAV failed for " << path << ": " << Mix_GetError() << "\n";
            return;
        }
    }
    
    if (Mix_PlayChannel(-1, chunk, 0) == -1) {
        std::cerr << "[Audio] Mix_PlayChannel failed: " << Mix_GetError() << "\n";
        Mix_FreeChunk(chunk);
    }
#else
    const char *soundNames[] = {
        "laser", "explosion", "error", "fail", "victory"
    };
    if (id >= 0 && id < SOUND_COUNT) {
        std::cout << "[Audio] (stub) play: " << soundNames[id] << "\n";
    }
#endif
}

void Audio::playMusic(const std::string &path) {
    if (!initialized) init();
#ifdef AUDIO_HAVE_SDL
    // Stop and free previous music if any
    if (s_music) {
        Mix_HaltMusic();
        Mix_FreeMusic((Mix_Music*)s_music);
        s_music = nullptr;
    }
    Mix_Music *m = Mix_LoadMUS(path.c_str());
    if (!m) {
        std::cerr << "[Audio] Mix_LoadMUS failed for " << path << ": " << Mix_GetError() << "\n";
        // fallback to stub logging
        std::ifstream f(path.c_str());
        if (!f.good()) std::cerr << "[Audio] Warning: file not found: " << path << "\n";
        currentMusicPath = path;
        musicPlaying = true;
        return;
    }
    s_music = (void*)m;
    if (Mix_PlayMusic(m, -1) == -1) {
        std::cerr << "[Audio] Mix_PlayMusic failed: " << Mix_GetError() << "\n";
    }
    currentMusicPath = path;
    musicPlaying = true;
#else
    // No SDL: just record path and log for debugging
    std::ifstream f(path.c_str());
    if (!f.good()) {
        std::cerr << "[Audio] Warning: music file not found: " << path << "\n";
    }
    currentMusicPath = path;
    musicPlaying = true;
    std::cout << "[Audio] (stub) playMusic: " << path << "\n";
#endif
}

void Audio::stopMusic() {
    if (!initialized) init();
#ifdef AUDIO_HAVE_SDL
    if (s_music) {
        Mix_HaltMusic();
        Mix_FreeMusic((Mix_Music*)s_music);
        s_music = nullptr;
    }
    musicPlaying = false;
#else
    if (musicPlaying) std::cout << "[Audio] (stub) stopMusic: " << currentMusicPath << "\n";
    currentMusicPath = "";
    musicPlaying = false;
#endif
}

void Audio::cleanup() {
    // Stop music and free resources if SDL_mixer is available
#ifdef AUDIO_HAVE_SDL
    if (s_music) {
        Mix_HaltMusic();
        Mix_FreeMusic((Mix_Music*)s_music);
        s_music = nullptr;
    }
    Mix_CloseAudio();
    SDL_Quit();
#endif
    initialized = false;
}

extern "C" void audio_cleanup_at_exit() {
    Audio::getInstance().cleanup();
}
