
#pragma once
#include <string>
#include <map>
#include "SDL_mixer.h"

class AudioManager {
public:
    static void Init();
    static void Clean();

    static void LoadSound(const std::string& id, const std::string& path);
    static void LoadMusic(const std::string& id, const std::string& path);

    static void PlaySound(const std::string& id, int loops = 0);
    static void PlayMusic(const std::string& id, int loops = -1);
    static void StopMusic();

private:
    static std::map<std::string, Mix_Chunk*> sounds;
    static std::map<std::string, Mix_Music*> music;
};