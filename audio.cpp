#include "audio.h"
#include <iostream>

std::map<std::string, Mix_Chunk*> AudioManager::sounds;
std::map<std::string, Mix_Music*> AudioManager::music;

void AudioManager::Init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer Error: " << Mix_GetError() << std::endl;
    }
}

void AudioManager::Clean() {
    for (auto& s : sounds) {
        Mix_FreeChunk(s.second);
    }
    for (auto& m : music) {
        Mix_FreeMusic(m.second);
    }
    sounds.clear();
    music.clear();
    Mix_CloseAudio();
}

void AudioManager::LoadSound(const std::string& id, const std::string& path) {
    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
    if (sound) {
        sounds[id] = sound;
    }
    else {
        std::cout << "Failed to load sound: " << Mix_GetError() << std::endl;
    }
}

void AudioManager::LoadMusic(const std::string& id, const std::string& path) {
    Mix_Music* m = Mix_LoadMUS(path.c_str());
    if (m) {
        music[id] = m;
    }
    else {
        std::cout << "Failed to load music: " << Mix_GetError() << std::endl;
    }
}

void AudioManager::PlaySound(const std::string& id, int loops) {
    Mix_PlayChannel(-1, sounds[id], loops);
}

void AudioManager::PlayMusic(const std::string& id, int loops) {
    Mix_PlayMusic(music[id], loops);
}

void AudioManager::StopMusic() {
    Mix_HaltMusic();
}