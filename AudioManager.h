#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include <iostream>
#include <vector>
#include <random>

class AudioManager {
public:
    static AudioManager& getInstance();
    bool init();
    void playMusic(const char* path, int loops = -1);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(int volume); // 0-128
    void playSoundEffect(const char* path);
    void playRandomHitSound();
    void playMissSound();
    void playRandomParrySound();
    void cleanup();

private:
    AudioManager();
    ~AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    Mix_Music* backgroundMusic;
    std::vector<Mix_Chunk*> soundEffects;
    std::vector<std::string> hitSoundPaths;
    std::vector<std::string> parrySoundPaths;
    static AudioManager* instance;
    std::mt19937 rng;
};