#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <string>
#include <iostream>
#include <vector>
#include <random>

// Singleton class to manage all game audio
class AudioManager {
public:
    static AudioManager& getInstance();
    bool init();
    
    // Music control functions
    void playMusic(const char* path, int loops = -1);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    void setMusicVolume(int volume); // 0-128
    void setNextTrack(const char* path);
    
    // Sound effect functions
    void playSoundEffect(const char* path);
    void playRandomHitSound();
    void playRandomParrySound();
    void playRandomDashSound();
    void playMissSound();
    void pauseAllSounds();
    void resumeAllSounds();
    
    // System functions
    void cleanup();
    void update();

private:
    // Singleton implementation
    AudioManager();
    ~AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;
    static AudioManager* instance;

    // Music properties
    Mix_Music* backgroundMusic;
    const char* nextMusicPath;
    Uint32 musicStartTime;
    bool waitingForNextTrack;
    static const Uint32 FIRST_TRACK_DURATION = 28000; // 28 seconds
    
    // Sound effect properties
    std::vector<Mix_Chunk*> soundEffects;
    std::vector<std::string> hitSoundPaths;
    std::vector<std::string> parrySoundPaths;
    std::vector<std::string> dashSoundPaths;
    
    // Random number generator for sound variety
    std::mt19937 rng;
};