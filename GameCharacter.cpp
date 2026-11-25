#include "GameCharacter.h"

GameCharacter::GameCharacter(SDL_Renderer* _renderer, std::string _path, float _tileSize)
{
	ChangeTexture(_renderer, _path);
	SetSize(_tileSize, _tileSize);
}

GameCharacter::~GameCharacter()
{
	// Release the texture loaded in the constructor
	SDL_DestroyTexture(Texture);
}

void GameCharacter::MoveToTile(Tile* _tile)
{
	// Snap the character's Rect onto the new tile's Rect, and remember where we are
	CurrentTile = _tile;
	Rect.x = _tile->Rect.x;
	Rect.y = _tile->Rect.y;
}

void GameCharacter::SetSize(float _w, float _h)
{
	Rect.w = _w;
	Rect.h = _h;
}

void GameCharacter::ChangeTexture(SDL_Renderer* _renderer, std::string _path)
{
	// Free the old texture first (if we have one), then load the new image
	if (Texture != nullptr)
	{
		SDL_DestroyTexture(Texture);
	}

	ImagePath = _path;
	Texture = IMG_LoadTexture(_renderer, ImagePath.c_str());
	if (Texture == nullptr)
	{
		std::cout << "Failed to load texture: " << ImagePath << "\n";
	}
	SDL_SetTextureScaleMode(Texture, SDL_SCALEMODE_NEAREST);
}

void GameCharacter::TakeDamage(int _amount)
{
	Health -= _amount;
	std::cout << Name << " takes " << _amount << " damage. Health left: " << Health << "\n";

	if (Health <= 0)
	{
		Health = 0;
		IsAlive = false;
		std::cout << Name << " has been defeated!\n";
	}
}

SDL_FlipMode GameCharacter::GetFlipMode()
{
	return this->flip;
}
