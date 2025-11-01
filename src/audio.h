#ifndef AUDIO_H
#define AUDIO_H

#include <string>

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

    bool init();
    void loadAll();
    void play(SoundId id);
    void cleanup();

private:
    Audio();
    ~Audio();
    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    bool initialized;
    void* chunks[SOUND_COUNT]; 
};

extern "C" void audio_cleanup_at_exit();

#endif // AUDIO_H
