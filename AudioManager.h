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
    void pauseAllSounds();
    void resumeAllSounds();
    void setMusicVolume(int volume); // 0-128
    void playSoundEffect(const char* path);
    void playRandomHitSound();
    void playMissSound();
    void playRandomParrySound();
    void playRandomDashSound();  // Add new method for dash sounds
    void cleanup();
    void update(); // Add update method to check music state
    void setNextTrack(const char* path); // Add method to set next track

private:
    AudioManager();
    ~AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    Mix_Music* backgroundMusic;
    std::vector<Mix_Chunk*> soundEffects;
    std::vector<std::string> hitSoundPaths;
    std::vector<std::string> parrySoundPaths;
    std::vector<std::string> dashSoundPaths;  // Add dash sound paths
    static AudioManager* instance;
    std::mt19937 rng;
    
    // Next track properties
    const char* nextMusicPath;
    Uint32 musicStartTime;
    bool waitingForNextTrack;
    static const Uint32 FIRST_TRACK_DURATION = 28000; // 28 seconds in milliseconds
};