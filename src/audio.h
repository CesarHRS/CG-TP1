#ifndef AUDIO_H
#define AUDIO_H

#include <string>

// Simple Audio manager using SDL2_mixer
// Place your sound files under assets/sounds/, filenames used below are examples.
class Audio {
public:
    enum SoundId {
        SOUND_LASER = 0,
        SOUND_EXPLOSION,
        SOUND_DAMAGE,
        SOUND_ERROR,
        SOUND_VICTORY,
        SOUND_COUNT
    };

    static Audio& getInstance();

    // Initialize audio subsystem. Returns true if initialized.
    bool init();
    // Load all configured sounds. Safe to call even if init failed (no-op).
    void loadAll();
    // Play a sound (no-op if sound not loaded)
    void play(SoundId id);
    // Cleanup resources
    void cleanup();

private:
    Audio();
    ~Audio();
    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    // internal state
    bool initialized;
    void* chunks[SOUND_COUNT]; // opaque pointer to Mix_Chunk to avoid including SDL headers here
};

// atexit helper declared so menu can register it
extern "C" void audio_cleanup_at_exit();

#endif // AUDIO_H
