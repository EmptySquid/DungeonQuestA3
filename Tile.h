#pragma once
#include <SDL3/SDl.h>
#include "Enums.h"

class Tile
{
public:
	//Variables
	bool Walkable;
	SDL_Texture* Texture;
	SDL_FRect Rect;
	int CoordinateX = 0;	// Grid position in the room, eg. Tiles[3][5]
	int CoordinateY = 0;
	PickupType TilePickup = NoPickup;	// Is there something lying on this tile?
	Tile* NorthNeighbour;
	Tile* EastNeighbour;
	Tile* SouthNeighbour;
	Tile* WestNeighbour;

	//Functions
	bool CompareColours(SDL_Color& _col1, SDL_Color& _col2);
	void Configure(SDL_Color& _color, float _x, float _y, float _size, SDL_Texture* _textures[], int _textureCount);
	void SetCoordinates(int _x, int _y);
	void SetNeighbours(Tile* _north, Tile* _east, Tile* _south, Tile* _west);
	Tile* GetNeighbour(Direction _direction);
};
