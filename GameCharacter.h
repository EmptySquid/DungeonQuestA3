#pragma once
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>
#include <iostream>
#include "Enums.h"
#include "Tile.h"

class GameCharacter
{
public:
	std::string Name = "Character";	// Used in the combat messages, eg. "Goblin"
	std::string ImagePath;
	SDL_Texture* Texture = nullptr;
	SDL_FRect Rect;
	Tile* CurrentTile = nullptr;	// The tile this character is standing on

	// Combat stats
	int Health = 1;
	int AttackPower = 0;	// Added to the dice roll in a fight
	bool IsAlive = true;
	SDL_FlipMode flip;

	GameCharacter(SDL_Renderer* _renderer, std::string _path, float _tileSize);
	~GameCharacter();
	void MoveToTile(Tile* _tile);
	void SetSize(float _w, float _h);
	void ChangeTexture(SDL_Renderer* _renderer, std::string _path);
	void TakeDamage(int _amount);
	SDL_FlipMode GetFlipMode();
};
