#pragma once
#include "GameCharacter.h"

// Forward declaration - tells the compiler "a class called DungeonGame exists, trust me".
// We can't #include "DungeonGame.h" here, because DungeonGame.h already includes Enemy.h.
// Each header would need the other one first (a circular dependency) and it won't compile.
// A pointer to DungeonGame is all we need in this header, so the forward declaration is enough.
// Enemy.cpp is free to #include "DungeonGame.h" because nothing includes a .cpp file.
class DungeonGame;

class Enemy :
    public GameCharacter
{
public:
    DungeonGame* GameRef = nullptr;     // So we can send our move requests to the game
    float MoveEverySeconds = 2;

    Enemy(SDL_Renderer* _renderer, std::string _path, float _tileSize, DungeonGame* _game);
    void Update(double _deltaTime);

private:
    double moveCountDown = 0;
    void DoRandomMove();
};
