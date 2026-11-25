#include "Tile.h"
#include <iostream>

bool Tile::CompareColours(SDL_Color& _col1, SDL_Color& _col2)
{
	return _col1.r == _col2.r && _col1.g == _col2.g && _col1.b == _col2.b;
}

void Tile::Configure(SDL_Color& _color, float _x, float _y, float _size, SDL_Texture* _textures[], int _textureCount)
{
	// Black pixels in the room file are walls, everything else is floor
	SDL_Color black = { 0, 0, 0, 255 };
	Walkable = !CompareColours(_color, black);

	if (Walkable)
	{
		// 80% plain carpet, 20% a random decorated tile
		if (SDL_rand(100) < 80)
			this->Texture = _textures[0];
		else
			this->Texture = _textures[1 + SDL_rand(_textureCount - 1)];
	}
	else
	{
		// No texture for walls
		this->Texture = nullptr;
	}

	// Clear any pickup left over from the last room loaded into this tile
	TilePickup = NoPickup;

	Rect.x = _x;
	Rect.y = _y;
	Rect.w = _size;
	Rect.h = _size;
}

void Tile::SetCoordinates(int _x, int _y)
{
	CoordinateX = _x;
	CoordinateY = _y;
}

void Tile::SetNeighbours(Tile* _north, Tile* _east, Tile* _south, Tile* _west)
{
	NorthNeighbour = _north;
	EastNeighbour = _east;
	SouthNeighbour = _south;
	WestNeighbour = _west;
}

Tile* Tile::GetNeighbour(Direction _direction)
{
	// Returns nullptr if there is no tile in that direction (the edge of the room)
	switch (_direction)
	{
		case North: return NorthNeighbour;
		case East:  return EastNeighbour;
		case South: return SouthNeighbour;
		case West:  return WestNeighbour;
	}
	return nullptr;
}
