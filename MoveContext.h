#pragma once
#include "Enums.h"
#include "Tile.h"
#include "GameCharacter.h"

// A simple 'data class' that holds everything we learn when we try to move.
// C++ functions can only return one thing, so TryMove returns one of these.
class MoveContext
{
public:
	MoveResult Result = Blocked;
	Tile* Destination = nullptr;		// The tile we tried to move onto (nullptr if it's outside the room)
	GameCharacter* Occupant = nullptr;	// Whoever is already standing on the destination tile (a goblin or the hero)
};
