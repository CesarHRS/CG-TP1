#include "audio.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

// SDL2 stubbed out - no audio functionality

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
    // Stub: no SDL2, just mark as initialized
    initialized = true;
    return true;
}

void Audio::loadAll() {
    // Stub: no sounds to load
    if (!initialized) {
        init();
    }
}

void Audio::play(SoundId id) {
    // Stub: no sound playback
    (void)id;
}

void Audio::cleanup() {
    // Stub: nothing to cleanup
    initialized = false;
}

extern "C" void audio_cleanup_at_exit() {
    Audio::getInstance().cleanup();
}
