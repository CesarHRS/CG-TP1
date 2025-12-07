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
    // Play a music file (looping). This is best-effort; when no mixer is
    // available this will be a no-op that logs the requested file.
    void playMusic(const std::string &path);
    // Set music volume (0.0 = silent, 1.0 = max)
    void setMusicVolume(float volume);
    // Stop currently playing music (if any)
    void stopMusic();
    void cleanup();

private:
    Audio();
    ~Audio();
    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    bool initialized;
    void* chunks[SOUND_COUNT]; 
    float musicVolume;
    // Current music state (stub-friendly)
    std::string currentMusicPath;
    bool musicPlaying;
};

extern "C" void audio_cleanup_at_exit();

#endif // AUDIO_H
