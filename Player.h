#pragma once
#include "GameCharacter.h"
class Player :
    public GameCharacter
{
public:
    int MaxHealth = 5;
    bool HasSword = false;

    // Passes everything up to the GameCharacter constructor
    Player(SDL_Renderer* _renderer, std::string _path, float _tileSize)
        : GameCharacter(_renderer, _path, _tileSize)
    {
        Name = "Hero";
    }

    void Heal(int _amount);
};
