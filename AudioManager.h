#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include <iostream>

class AudioManager {
public:
    static AudioManager& getInstance();
    bool init();
    void playMusic(const char* path, int loops = -1);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(int volume); // 0-128
    void cleanup();

private:
    AudioManager() : backgroundMusic(nullptr) {} // Private constructor for singleton
    ~AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    Mix_Music* backgroundMusic;
    static AudioManager* instance;
};