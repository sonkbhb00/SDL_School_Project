#include "AudioManager.h"

AudioManager* AudioManager::instance = nullptr;

AudioManager& AudioManager::getInstance() {
    if (instance == nullptr) {
        instance = new AudioManager();
    }
    return *instance;
}

AudioManager::~AudioManager() {
    cleanup();
}

bool AudioManager::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return false;
    }
    return true;
}

void AudioManager::playMusic(const char* path, int loops) {
    stopMusic(); // Stop any currently playing music
    
    backgroundMusic = Mix_LoadMUS(path);
    if (backgroundMusic == nullptr) {
        std::cout << "Failed to load music! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return;
    }
    
    if (Mix_PlayMusic(backgroundMusic, loops) < 0) {
        std::cout << "Failed to play music! SDL_mixer Error: " << Mix_GetError() << std::endl;
    }
}

void AudioManager::stopMusic() {
    if (backgroundMusic != nullptr) {
        Mix_HaltMusic();
        Mix_FreeMusic(backgroundMusic);
        backgroundMusic = nullptr;
    }
}

void AudioManager::pauseMusic() {
    if (Mix_PlayingMusic()) {
        Mix_PauseMusic();
    }
}

void AudioManager::resumeMusic() {
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
    }
}

void AudioManager::setMusicVolume(int volume) {
    Mix_VolumeMusic(volume);
}

void AudioManager::cleanup() {
    stopMusic();
    Mix_CloseAudio();
    Mix_Quit();
}