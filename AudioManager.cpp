#include "AudioManager.h"
#include <random>

AudioManager* AudioManager::instance = nullptr;

AudioManager::AudioManager() : backgroundMusic(nullptr), nextMusicPath(nullptr),
    musicStartTime(0), waitingForNextTrack(false) {
    // Initialize random number generator
    std::random_device rd;
    rng.seed(rd());

    // Initialize hit sound paths
    hitSoundPaths = {
        "audio/11L-a_sword_cut_through_-1744557094287.mp3",
        "audio/11L-a_sword_cut_through_-1744557088332.mp3",
        "audio/11L-a_sword_cut_through_-1744557091173.mp3",
        "audio/11L-a_sword_cut_through_-1744557092641.mp3"
    };

    // Initialize parry sound paths
    parrySoundPaths = {
        "audio/11L-two_big_sword_clashe-1744557100460.mp3",
        "audio/11L-two_big_sword_clashe-1744557101819.mp3",
        "audio/11L-two_big_sword_clashe-1744557097181.mp3",
        "audio/11L-two_big_sword_clashe-1744557099053.mp3"
    };

    // Initialize dash sound paths
    dashSoundPaths = {
        "audio/Yasuo_Original_EDash_0.ogg",
        "audio/Yasuo_Original_EDash_1.ogg",
        "audio/Yasuo_Original_EDash_2.ogg",
        "audio/Yasuo_Original_EDash_3.ogg"
    };
}

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
    
    musicStartTime = SDL_GetTicks();
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

void AudioManager::playSoundEffect(const char* path) {
    Mix_Chunk* effect = Mix_LoadWAV(path);
    if (!effect) {
        std::cout << "Failed to load sound effect! SDL_mixer Error: " << Mix_GetError() << std::endl;
        return;
    }
    
    Mix_PlayChannel(-1, effect, 0);
    soundEffects.push_back(effect); // Store for cleanup
}

void AudioManager::playRandomHitSound() {
    if (hitSoundPaths.empty()) return;
    
    std::uniform_int_distribution<int> dist(0, hitSoundPaths.size() - 1);
    int randomIndex = dist(rng);
    playSoundEffect(hitSoundPaths[randomIndex].c_str());
}

void AudioManager::playMissSound() {
    playSoundEffect("audio/11L-a_sword_swing_in_the-1744557232414.mp3");
}

void AudioManager::playRandomParrySound() {
    if (parrySoundPaths.empty()) return;
    
    std::uniform_int_distribution<int> dist(0, parrySoundPaths.size() - 1);
    int randomIndex = dist(rng);
    playSoundEffect(parrySoundPaths[randomIndex].c_str());
}

void AudioManager::pauseAllSounds() {
    // Pause all currently playing sound channels
    Mix_Pause(-1);  // -1 pauses all channels
}

void AudioManager::resumeAllSounds() {
    // Resume all paused sound channels
    Mix_Resume(-1);  // -1 resumes all channels
}

void AudioManager::setNextTrack(const char* path) {
    nextMusicPath = path;
    waitingForNextTrack = true;
}

void AudioManager::update() {
    if (waitingForNextTrack && backgroundMusic) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - musicStartTime >= FIRST_TRACK_DURATION) {
            playMusic(nextMusicPath);
            waitingForNextTrack = false;
            nextMusicPath = nullptr;
        }
    }
}

void AudioManager::cleanup() {
    stopMusic();
    
    // Cleanup all sound effects
    for (Mix_Chunk* effect : soundEffects) {
        Mix_FreeChunk(effect);
    }
    soundEffects.clear();
    
    Mix_CloseAudio();
    Mix_Quit();
}

void AudioManager::playRandomDashSound() {
    if (dashSoundPaths.empty()) return;
    
    std::uniform_int_distribution<int> dist(0, dashSoundPaths.size() - 1);
    int randomIndex = dist(rng);
    playSoundEffect(dashSoundPaths[randomIndex].c_str());
}